#include "metric_pairf.h"
using namespace n3;

// Count in same way as ISBI Challenge 2012
void n3::getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
			std::map<LabelPair, BigInt> const& overlaps)
{
  tp  = 0;
  BigInt nPoint = 0;
  std::set<Label> resLabels, refLabels;
  for (std::map<LabelPair, BigInt>::const_iterator it = overlaps.begin(); 
       it != overlaps.end(); ++it) {
    nPoint += it->second;
    Label resLabel = it->first.first, refLabel = it->first.second;
    resLabels.insert(resLabel);
    refLabels.insert(refLabel);
    if (resLabel != BGVAL && refLabel != BGVAL)
      tp += (it->second * (it->second - 1) / 2);
  }
  BigInt nPair = nPoint * (nPoint - 1) / 2;
  BigInt nPairSameRefLabel = 0;
  for (std::set<Label>::const_iterator rfit = refLabels.begin(); 
       rfit != refLabels.end(); ++rfit) {
    BigInt nPointOneRefLabel = 0;
    for (std::set<Label>::const_iterator rsit = resLabels.begin(); 
	 rsit != resLabels.end(); ++rsit) {
      LabelPair key = std::make_pair(*rsit, *rfit);
      std::map<LabelPair, BigInt>::const_iterator oit = overlaps.find(key);
      if (oit != overlaps.end()) nPointOneRefLabel += oit->second;
    }
    nPairSameRefLabel += 
      (nPointOneRefLabel * (nPointOneRefLabel - 1) / 2);
  }
  BigInt nPairSameResLabel = 0;
  for (std::set<Label>::const_iterator rsit = resLabels.begin(); 
       rsit != resLabels.end(); ++rsit) 
    if (*rsit != BGVAL) {
      BigInt nPointOneResLabel = 0;
      for (std::set<Label>::const_iterator rfit = refLabels.begin(); 
	   rfit != refLabels.end(); ++rfit) {
	LabelPair key = std::make_pair(*rsit, *rfit);
	std::map<LabelPair, BigInt>::const_iterator oit = 
	  overlaps.find(key); 
	if (oit != overlaps.end()) nPointOneResLabel += oit->second;
      }
      nPairSameResLabel += 
	(nPointOneResLabel * (nPointOneResLabel - 1) / 2);
    }
  tn = nPair - nPairSameRefLabel + tp - nPairSameResLabel;
  fp = nPairSameResLabel - tp;
  fn = nPairSameRefLabel - tp;
}



BigFloat n3::getPrecision (BigInt const& tp, BigInt const& fp, int prec)
{  
  BigFloat denom(tp + fp, prec);
  return BigFloat(tp, prec) / 
    (denom >= EPSILON? denom: (denom + EPSILON));
}



BigFloat n3::getRecall (BigInt const& tp, BigInt const& fn, int prec)
{
  BigFloat denom(tp + fn, prec);
  return BigFloat(tp, prec) / 
    (denom >= EPSILON? denom: (denom + EPSILON));
}



BigFloat n3::getF (BigInt const& tp, BigInt const& fp, BigInt const& fn, 
		   int prec)
{
  BigFloat denom(2 * tp + fp + fn), num(2 * tp);
  return num / (denom >= EPSILON? denom: (denom + EPSILON));
}



BigFloat n3::getF (BigFloat const& precision, BigFloat const& recall)
{
  BigFloat denom(precision + recall);
  return precision * recall * 2.0 / 
    (denom >= EPSILON? denom: (denom + EPSILON));
}



// bg: background points, set to NULL if none
void n3::getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
			std::list<const Points*> const& segs, 
			const Points* bg, LabelImage::Pointer refImage)
{
  std::map<LabelPair, BigInt> overlaps;
  getOverlaps(overlaps, segs, bg, refImage, false);
  getPairScores(tp, tn, fp, fn, overlaps);
}



// bg: background points, set to NULL if none
void n3::getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
			std::list<const Points3*> const& segs, 
			const Points3* bg, LabelImage3::Pointer refImage)
{
  std::map<LabelPair, BigInt> overlaps;
  getOverlaps(overlaps, segs, bg, refImage, false);
  getPairScores(tp, tn, fp, fn, overlaps);
}



void n3::getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
			LabelImage::Pointer resImage, 
			LabelImage::Pointer refImage)
{
  std::map<LabelPair, BigInt> overlaps;
  std::vector<LabelImage::Pointer> resContainer, refContainer;
  resContainer.push_back(resImage);
  refContainer.push_back(refImage);
  getOverlaps(overlaps, resContainer, refContainer, false);
  getPairScores(tp, tn, fp, fn, overlaps);
}



void n3::getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
			LabelImage3::Pointer resImage, 
			LabelImage3::Pointer refImage)
{
  std::map<LabelPair, BigInt> overlaps;
  std::vector<LabelImage3::Pointer> resContainer, refContainer;
  resContainer.push_back(resImage);
  refContainer.push_back(refImage);
  getOverlaps(overlaps, resContainer, refContainer, false);
  getPairScores(tp, tn, fp, fn, overlaps);
}



// Inputs are 3D volumes
void n3::getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
			std::vector<LabelImage::Pointer> const& resVolume, 
			std::vector<LabelImage::Pointer> const& refVolume)
{
  std::map<LabelPair, BigInt> overlaps;
  getOverlaps(overlaps, resVolume, refVolume, false);
  getPairScores(tp, tn, fp, fn, overlaps);  
} 



// Inputs are 3D volumes
void n3::getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
			std::vector<LabelImage3::Pointer> const& resVolume, 
			std::vector<LabelImage3::Pointer> const& refVolume)
{
  std::map<LabelPair, BigInt> overlaps;
  getOverlaps(overlaps, resVolume, refVolume, false);
  getPairScores(tp, tn, fp, fn, overlaps);  
} 



// Compute pair precision/recall/F-score
// bg: background points, set to NULL if none
std::vector<BigFloat> 
n3::getPairScores (std::list<const Points*> const& segs, 
		   const Points* bg, LabelImage::Pointer refImage, 
		   int prec)
{
  BigInt tp, tn, fp, fn;
  getPairScores(tp, tn, fp, fn, segs, bg, refImage);
  std::vector<BigFloat> ret;
  ret.reserve(3);
  ret.push_back(getPrecision(tp, fp, prec));
  ret.push_back(getRecall(tp, fn, prec));
  ret.push_back(getF(tp, fp, fn, prec));
  return ret;
}



// Compute pair precision/recall/F-score
// bg: background points, set to NULL if none
std::vector<BigFloat> 
n3::getPairScores (std::list<const Points3*> const& segs, 
		   const Points3* bg, LabelImage3::Pointer refImage, 
		   int prec)
{
  BigInt tp, tn, fp, fn;
  getPairScores(tp, tn, fp, fn, segs, bg, refImage);
  std::vector<BigFloat> ret;
  ret.reserve(3);
  ret.push_back(getPrecision(tp, fp, prec));
  ret.push_back(getRecall(tp, fn, prec));
  ret.push_back(getF(tp, fp, fn, prec));
  return ret;
}
