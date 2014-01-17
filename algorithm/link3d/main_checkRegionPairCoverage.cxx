#include "util/struct_util.h"
using namespace n3;

void operation (std::vector<const char*> const& segImageNames2, 
		std::vector<const char*> const& segImageNames3, 
		double cdth)
{
  int n = segImageNames2.size();
  if (n < 3) perr("Error: need at least 3 images...");
  LabelImage::Pointer 
    segImage20 = readImage<LabelImage>(segImageNames2[0]), 
    segImage21 = readImage<LabelImage>(segImageNames2[1]), 
    segImage22 = readImage<LabelImage>(segImageNames2[2]),
    segImage30 = readImage<LabelImage>(segImageNames3[0]), 
    segImage31 = readImage<LabelImage>(segImageNames3[1]), 
    segImage32 = readImage<LabelImage>(segImageNames3[2]);
  std::map<Label, Label> lmap230, lmap231, lmap232;
  itk::ImageRegionConstIterator<LabelImage> 
    it20(segImage20, segImage20->GetLargestPossibleRegion()), 
    it21(segImage21, segImage21->GetLargestPossibleRegion()), 
    it22(segImage22, segImage22->GetLargestPossibleRegion()), 
    it30(segImage30, segImage30->GetLargestPossibleRegion()), 
    it31(segImage31, segImage31->GetLargestPossibleRegion()),
    it32(segImage32, segImage31->GetLargestPossibleRegion());
  while (!it20.IsAtEnd()) {
    if (it20.Get() != BGVAL && it30.Get() != BGVAL)
      lmap230[it20.Get()] = it30.Get();
    ++it20;
    ++it30;
  }
  while (!it21.IsAtEnd()) {
    if (it21.Get() != BGVAL && it31.Get() != BGVAL)
      lmap231[it21.Get()] = it31.Get();
    ++it21;
    ++it31;
  }
  while (!it22.IsAtEnd()) {
    if (it22.Get() != BGVAL && it32.Get() != BGVAL)
      lmap232[it22.Get()] = it32.Get();
    ++it22;
    ++it32;
  }
  PointMap rmap20, rmap21, rmap22, rmap30, rmap31, rmap32;
  getPointMap(rmap20, segImage20, false);
  getPointMap(rmap21, segImage21, false);
  getPointMap(rmap22, segImage22, false);
  getPointMap(rmap30, segImage30, false);
  getPointMap(rmap31, segImage31, false);
  getPointMap(rmap32, segImage32, false);
  std::map<Label, Point> cmap20, cmap21, cmap22;
  for (PointMap::const_iterator rit0 = rmap20.begin(); 
       rit0 != rmap20.end(); ++rit0)
    cmap20[rit0->first] = rit0->second.get_centroid();
  for (PointMap::const_iterator rit1 = rmap21.begin(); 
       rit1 != rmap21.end(); ++rit1)
    cmap21[rit1->first] = rit1->second.get_centroid();
  for (PointMap::const_iterator rit2 = rmap22.begin(); 
       rit2 != rmap22.end(); ++rit2)
    cmap22[rit2->first] = rit2->second.get_centroid();
  for (int i = 1; i < n - 1; ++i) {
    for (PointMap::const_iterator rit1 = rmap21.begin(); 
	 rit1 != rmap21.end(); ++rit1) {
      Label l3 = lmap231.find(rit1->first)->second;
      if (rmap31.count(l3) == 0) continue;
      Point c1 = cmap21.find(rit1->first)->second;
      std::set<Label> rset0;
      getOverlaps(rset0, rit1->second, segImage20, false);
      for (std::map<Label, Point>::const_iterator cit0 = cmap20.begin(); 
	   cit0 != cmap20.end(); ++cit0)
	if (c1.get_distance(cit0->second) <= cdth) 
	  rset0.insert(cit0->first);
      bool found = false;
      for (std::set<Label>::const_iterator rsit = rset0.begin(); 
	   rsit != rset0.end(); ++rsit)
	if (lmap230.find(*rsit)->second == l3) {
	  found = true;
	  break;
	}
      if (!found) {
	std::set<Label> rset2;
	getOverlaps(rset2, rit1->second, segImage22, false);
	for (std::map<Label, Point>::const_iterator cit2 = cmap22.begin(); 
	     cit2 != cmap22.end(); ++cit2)
	  if (c1.get_distance(cit2->second) <= cdth) 
	    rset2.insert(cit2->first);
	for (std::set<Label>::const_iterator rsit = rset2.begin(); 
	     rsit != rset2.end(); ++rsit)
	  if (lmap232.find(*rsit)->second == l3) {
	    found = true;
	    break;
	  }
      }
      if (!found) {
	std::cerr << "Out of coverage: slice =  " << i << ", region = "
		  << rit1->first << std::endl;
	return;
      }
    }
    if (i < n - 2) {
      segImage20 = segImage21;
      segImage21 = segImage22;
      segImage22 = readImage<LabelImage>(segImageNames2[i + 2]);
      segImage30 = segImage31;
      segImage31 = segImage32;
      segImage32 = readImage<LabelImage>(segImageNames3[i + 2]);
      lmap230 = lmap231;
      lmap231 = lmap232;
      lmap232.clear();
      itk::ImageRegionConstIterator<LabelImage> 
	it21n(segImage21, segImage21->GetLargestPossibleRegion()), 
	it22n(segImage22, segImage22->GetLargestPossibleRegion()), 
	it31n(segImage31, segImage31->GetLargestPossibleRegion()),
	it32n(segImage32, segImage31->GetLargestPossibleRegion());
      while (!it22n.IsAtEnd()) {
	if (it22n.Get() != BGVAL && it32n.Get() != BGVAL)
	  lmap232[it22n.Get()] = it32n.Get();
	++it22n;
	++it32n;
      }
      rmap20 = rmap21;
      rmap21 = rmap22;
      rmap22.clear();
      rmap30 = rmap31;
      rmap31 = rmap32;
      rmap32.clear();
      getPointMap(rmap22, segImage22, false);
      getPointMap(rmap32, segImage32, false);
      cmap20 = cmap21;
      cmap21 = cmap22;
      cmap22.clear();
      for (PointMap::const_iterator rit2 = rmap22.begin(); 
	   rit2 != rmap22.end(); ++rit2)
	cmap22[rit2->first] = rit2->second.get_centroid();      
    }
  }
}



int main (int argc, char* argv[])
{
  if (argc < 7) {
    std::cerr << "Usage: " << argv[0] 
	      << " imageNum"
	      << " startSliceNo"
	      << " sliceNoInc" 
	      << " labelImageNameTemplate2"
	      << " labelImageNameTemplate3"
	      << " centroidDistanceThreshold"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  int n = atoi(argv[argi++]);
  int startSliceNo = atoi(argv[argi++]);
  int sliceNoInc = atoi(argv[argi++]);
  const char* segImageNameTemplate2 = argv[argi++];
  const char* segImageNameTemplate3 = argv[argi++];
  std::vector<const char*> segImageNames2, segImageNames3;
  segImageNames2.reserve(n);
  segImageNames3.reserve(n);
  for (int i = 0; i < n; ++i) {
    char* name2 = new char[1000]; 
    char* name3 = new char[1000];
    sprintf(name2, segImageNameTemplate2, startSliceNo + sliceNoInc * i);
    sprintf(name3, segImageNameTemplate3, startSliceNo + sliceNoInc * i);
    segImageNames2.push_back(name2);
    segImageNames3.push_back(name3);
  }
  double cdth = atof(argv[argi++]);
  operation(segImageNames2, segImageNames3, cdth);
  for (int i = 0; i < n; ++i) {
    delete[] segImageNames2[i];
    delete[] segImageNames3[i];
  }
  return EXIT_SUCCESS;
}
