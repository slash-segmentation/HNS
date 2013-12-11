#include "util/struct_util.h"
#include "util/cv/cv_contour.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		bool write16) 
{
  LabelImage::Pointer image = readImage<LabelImage>(inputImageName);
  PointMap rmap;
  getPointMap(rmap, image, true);
  Points openSet = rmap[BGVAL];
  rmap.erase(BGVAL);
  while (openSet.size() > 0) {
    Points changeSet;
    std::list<Label> changeToLabels;
    for (Points::iterator pit = openSet.begin(); pit != openSet.end(); 
	 ++pit) {
      std::vector<Pixel<Label> > n = 
	getNeighbors<LabelImage>(*pit, image, 4);
      Label l = BGVAL;
      unsigned int a = UINT_MAX;
      for (std::vector<Pixel<Label> >::const_iterator nit = n.begin(); 
	   nit != n.end(); ++nit) {
	unsigned int thisa = rmap[nit->val].size();
	if (nit->val != BGVAL && thisa < a) {
	  a = thisa;
	  l = nit->val;
	}
      }
      if (l != BGVAL) {
	Points::iterator tpit = pit;
	--pit;
	changeSet.splice(changeSet.end(), openSet, tpit);
	changeToLabels.push_back(l);
      }
    }
    Points::const_iterator pit = changeSet.begin();
    std::list<Label>::const_iterator lit = changeToLabels.begin();
    while (pit != changeSet.end()) {
      setv<LabelImage>(image, pit->x, pit->y, *lit);
      ++pit;
      ++lit;
    }
  }
  if (write16) 
    writeImage<UInt16Image>(outputImageName, 
			    castImage<LabelImage, UInt16Image>(image));
  else writeImage<LabelImage>(outputImageName, image);
}



int main (int argc, char* argv[])
{
  if (argc != 3 && argc != 4) {
    std::cerr << "Usage: " << argv[0]
	      << " inputImageName" 
	      << " [writeToUInt16Image = 0]"
	      << " outputImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  bool write16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false);
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, write16);
  return EXIT_SUCCESS;
}
