#include "util/struct_util.h"
#include "link3d_io.h"
#include "link3d.h"
using namespace n3;
using namespace n3::link3d;

void operation (const char* regionPairFileName, 
		int sliceNo0, int sliceNo1, 
		const char* labelImageName0, 
		const char* labelImageName1, 
		double cdth)
{
  LabelImage::Pointer labelImage0 = readImage<LabelImage>(labelImageName0);
  LabelImage::Pointer labelImage1 = readImage<LabelImage>(labelImageName1);
  std::list<LabelPair> pairs;
  getOverlaps(pairs, labelImage0, labelImage1, false, false);
  if (cdth > 0) {
    PointMap rmap0, rmap1;
    getPointMap(rmap0, labelImage0, false);
    getPointMap(rmap1, labelImage1, false);
    PointMap cmap0, cmap1;
    for (PointMap::const_iterator it = rmap0.begin(); it != rmap0.end(); 
	 ++it) cmap0[it->first].push_back(it->second.get_centroid());
    for (PointMap::const_iterator it = rmap1.begin(); it != rmap1.end(); 
	 ++it) cmap1[it->first].push_back(it->second.get_centroid());
    std::list<LabelPair> cdpairs;
    for (PointMap::const_iterator it0 = cmap0.begin(); 
	 it0 != cmap0.end(); ++it0) 
      for (PointMap::const_iterator it1 = cmap1.begin(); 
	   it1 != cmap1.end(); ++it1) 
	if (it0->second.front().get_distance(it1->second.front()) <= cdth)
	  cdpairs.push_back(std::make_pair(it0->first, it1->first));
    cdpairs.sort();
    pairs.merge(cdpairs);
  }
  pairs.unique();
  std::list<fLink> links;
  for (std::list<LabelPair>::const_iterator it = pairs.begin(); 
       it != pairs.end(); ++it) 
    links.push_back(fLink(sliceNo0, it->first, sliceNo1, it->second));
  writeLinks(regionPairFileName, NULL, links);
}



int main (int argc, char* argv[])
{
  if (argc != 7) {
    std::cerr << "Usage: " << argv[0] 
	      << " sliceNo0" 
	      << " sliceNo1"
	      << " labelImageName0" 
	      << " labelImageName1" 
	      << " centroidDistanceThreshold ('0' to skip)"
	      << " regionPairFileName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  int sliceNo0 = atoi(argv[argi++]);
  int sliceNo1 = atoi(argv[argi++]);
  const char* labelImageName0 = argv[argi++];
  const char* labelImageName1 = argv[argi++];
  double cdth = atof(argv[argi++]);
  const char* regionPairFileName = argv[argi++];
  operation(regionPairFileName, sliceNo0, sliceNo1, 
	    labelImageName0, labelImageName1, cdth);
  return EXIT_SUCCESS;
}
