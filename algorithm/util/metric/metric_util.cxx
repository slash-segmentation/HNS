#include "metric_util.h"
using namespace n3;

void n3::getOverlaps (std::map<LabelPair, BigInt>& overlaps, 
		      std::vector<LabelImage::Pointer> const& resImages, 
		      std::vector<LabelImage::Pointer> const& refImages, 
		      bool includeRefBG)
{
  int n = refImages.size();
  if (n != resImages.size()) perr("Error: image numbers disagree...");
  for (int i = 0; i < n; ++i) 
    getOverlaps<BigInt>(overlaps, resImages[i], refImages[i], 
			true, includeRefBG);
}



void n3::getOverlaps (std::map<LabelPair, BigInt>& overlaps, 
		      std::vector<LabelImage3::Pointer> const& resImages, 
		      std::vector<LabelImage3::Pointer> const& refImages, 
		      bool includeRefBG)
{
  int n = refImages.size();
  if (n != resImages.size()) perr("Error: image numbers disagree...");
  for (int i = 0; i < n; ++i) 
    getOverlaps<BigInt>(overlaps, resImages[i], refImages[i], 
			true, includeRefBG);
}



/* Given a reference image and a set of regions count overlaps */
/* bg: background points, set to NULL if none */
void n3::getOverlaps (std::map<LabelPair, BigInt>& overlaps, 
		      std::list<const Points*> const& segs, 
		      const Points* bg, LabelImage::Pointer refImage, 
		      bool includeRefBG)
{
  Label res = 1;
  std::list<const Points*> clusters = segs;
  if (bg != NULL) {
    clusters.push_front(bg);
    res = BGVAL;
  }
  for (std::list<const Points*>::const_iterator sit = clusters.begin(); 
       sit != clusters.end(); ++sit) {
    for (Points::const_iterator pit = (*sit)->begin(); 
	 pit != (*sit)->end(); ++pit) {
      Label ref = getv<LabelImage>(refImage, pit->x, pit->y);
      if (ref != BGVAL || includeRefBG) {
	LabelPair key = std::make_pair(res, ref);
	std::map<LabelPair, BigInt>::iterator it = overlaps.find(key);
	if (it != overlaps.end()) ++it->second;
	else overlaps[key] = 1;
      }
    }
    ++res;
  }
}



/* Given a reference image and a set of regions count overlaps */
/* bg: background points, set to NULL if none */
void n3::getOverlaps (std::map<LabelPair, BigInt>& overlaps, 
		      std::list<const Points3*> const& segs, 
		      const Points3* bg, LabelImage3::Pointer refImage, 
		      bool includeRefBG)
{
  Label res = 1;
  std::list<const Points3*> clusters = segs;
  if (bg != NULL) {
    clusters.push_front(bg);
    res = BGVAL;
  }
  for (std::list<const Points3*>::const_iterator sit = clusters.begin(); 
       sit != clusters.end(); ++sit) {
    for (Points3::const_iterator pit = (*sit)->begin(); 
	 pit != (*sit)->end(); ++pit) {
      Label ref = getv<LabelImage3>(refImage, pit->x, pit->y, pit->z);
      if (ref != BGVAL || includeRefBG) {
	LabelPair key = std::make_pair(res, ref);
	std::map<LabelPair, BigInt>::iterator it = overlaps.find(key);
	if (it != overlaps.end()) ++it->second;
	else overlaps[key] = 1;
      }
    }
    ++res;
  }
}



/* Find a matching region from ref image for a region on src image  */
/* Return false if no matching is found, which means m is invalid */
/* Ignore background pixels on both images */
/* Match type is max overlap ratio */
bool n3::match (std::pair<Label, double>& m, Points const& srcRegion, 
		int srcSize, LabelImage::Pointer refImage, 
		std::map<Label, unsigned int> const& refSizeMap)
{
  std::map<Label, unsigned int> overlaps;
  getOverlaps(overlaps, srcRegion, refImage, false);
  if (overlaps.size() == 0) return false;
  m.first = overlaps.begin()->first;
  m.second = (double)overlaps.begin()->second / 
    (double)(refSizeMap.find(m.first)->second + 
	     srcSize - overlaps.begin()->second);
  for (std::map<Label, unsigned int>::const_iterator oit = 
	 overlaps.begin(); oit != overlaps.end(); ++oit) {
    Label ref;
    double r = (double)oit->second / 
      (double)(refSizeMap.find(ref)->second + srcSize - oit->second);
    if (r > m.second) {
      m.first = ref;
      m.second = r;
    }
  }
  return true;
}
