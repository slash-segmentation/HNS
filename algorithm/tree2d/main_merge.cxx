#include "tree2d_util.h"
using namespace n3;
using namespace n3::tree2d;

// Assume merging rfrom to rto
void merge (PointMap& rmap, PointLabelMap& lmap, Label rfrom, Label rto)
{
  Points c2rp;
  std::list<PointLabelMap::iterator> irm;
  for (PointLabelMap::iterator it = lmap.begin(); it != lmap.end(); ++it) {
    LabelSet::iterator fit = it->second.find(rfrom);
    LabelSet::iterator tit = it->second.find(rto);
    if (it->second.size() == 2 && fit != it->second.end() && 
	tit != it->second.end()) {
      c2rp.push_back(it->first);
      irm.push_back(it);
    }
    else if (fit != it->second.end()) {
      if (tit == it->second.end()) it->second.insert(rto);
      it->second.erase(fit);
    }
  }
  for (std::list<PointLabelMap::iterator>::const_iterator it = 
	 irm.begin(); it != irm.end(); ++it) lmap.erase(*it);
  splice(rmap[rto], rmap[rfrom], false);
  splice(rmap[rto], c2rp, false);
  rmap.erase(rfrom);
}



Label getMinBoundaryNeighbor (BoundaryTable const& bt, Label r0)
{
  Label r1 = BGVAL;
  Float vmin = FLOAT_MAX;
  for (BoundaryTable::const_iterator bit = bt.begin(); 
       bit != bt.end(); ++bit) {
    Label l0 = bit->first.first, l1 = bit->first.second;
    if (l0 == r0 || l1 == r0) {
      double v = -getSaliency(bit->second);
      if (v < vmin) {
	r1 = (r0 == l0? l1: l0);
	vmin = v;
      }
    }
  }
  return r1;
}



void operation (const char* outputImageName, const char* inputImageName, 
		const char* pbImageName, int ath0, int ath1, double pth, 
		bool write16, bool relabel)
{
  LabelImage::Pointer labelImage = readImage<LabelImage>(inputImageName);
  labelImage = relabelImage<LabelImage, LabelImage>(labelImage, 0);
  FloatImage::Pointer pbImage = readImage<FloatImage>(pbImageName);
  PointMap rmap, cmap;
  getPointMap(rmap, labelImage, true);
  PointLabelMap lmap;
  getPointNeighbors(lmap, rmap[BGVAL], labelImage, CRCONN);
  rmap.erase(BGVAL);
  BoundaryTable bt;
  getBoundaryTable(bt, lmap, pbImage);
  std::set<Label> aset, pset;
  for (PointMap::const_iterator it = rmap.begin(); it != rmap.end(); ++it) 
    if (it->second.size() <= ath0) aset.insert(it->first);
    else if (it->second.size() <= ath1) pset.insert(it->first);
  // Type 0 merge 
  while (aset.size() > 0) {
    std::set<Label>::iterator sit = aset.begin();
    Label rfrom = *sit;
    PointMap::const_iterator fit = rmap.find(rfrom);
    if (fit != rmap.end() && fit->second.size() <= ath0) {
      Label rto = getMinBoundaryNeighbor(bt, rfrom);
      if (rto != BGVAL) {
	updateBoundaryTable(bt, lmap, rfrom, rto);
	merge(rmap, lmap, rfrom, rto);
	PointMap::const_iterator tit = rmap.find(rto);
	if (tit->second.size() > ath1) {
	  aset.erase(rto);
	  pset.erase(rto);
	}
	else if (tit->second.size() > ath0) {
	  aset.erase(rto);
	  pset.insert(rto);
	}
	else aset.insert(rto);
      }
    }
    aset.erase(sit);
  }
  // Type 1 merge
  while (pset.size() > 0) {
    std::set<Label>::iterator sit = pset.begin();
    Label rfrom = *sit;
    PointMap::const_iterator fit = rmap.find(rfrom);
    if (fit != rmap.end() && fit->second.size() <= ath1) {
      std::list<Float> vals;
      getvs<FloatImage>(vals, pbImage, fit->second);
      Float v = getMean(vals);
      if (v >= pth) {
	Label rto = getMinBoundaryNeighbor(bt, rfrom);
	if (rto != BGVAL) {
	  updateBoundaryTable(bt, lmap, rfrom, rto);
	  merge(rmap, lmap, rfrom, rto);
	  PointMap::const_iterator tit = rmap.find(rto);
	  if (tit != rmap.end()) 
	    if (tit->second.size() > ath1) pset.erase(rto);
	    else pset.insert(rto);
	}
      }
    }
    pset.erase(sit);
  }
  labelImage->FillBuffer(BGVAL);
  for (PointMap::const_iterator it = rmap.begin(); it != rmap.end(); 
       ++it) setvs<LabelImage>(labelImage, it->second, it->first);
  if (write16) {
    UInt16Image::Pointer outputImage = 
      (relabel? relabelImage<LabelImage, UInt16Image>(labelImage, 0): 
       castImage<LabelImage, UInt16Image>(labelImage));
    writeImage<UInt16Image>(outputImageName, outputImage);
  }
  else {
    if (relabel) 
      labelImage = relabelImage<LabelImage, LabelImage>(labelImage, 0);
    writeImage<LabelImage>(outputImageName, labelImage);
  }
}



int main (int argc, char* argv[])
{
  if (argc < 7 || argc > 9) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputSegImageName" 
	      << " boundaryProbabilityImageName" 
	      << " areaThreshold0"
	      << " areaThreshold1" 
	      << " probabilityThreshold"
	      << " [writeToUInt16 = 0]"
	      << " [consecutiveOutputLabels = 1]"
	      << " outputSegImageName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  const char* pbImageName = argv[argi++];
  int ath0 = atoi(argv[argi++]);
  int ath1 = atoi(argv[argi++]);
  double pth = atof(argv[argi++]);
  bool write16 = 
    (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false);
  bool relabel = 
    (argi < argc - 1 && atoi(argv[argi++]) == 0? false: true);
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, pbImageName, 
	    ath0, ath1, pth, write16, relabel);
  return EXIT_SUCCESS;
}
