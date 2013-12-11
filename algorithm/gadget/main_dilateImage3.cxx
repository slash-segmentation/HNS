#include "util/struct_util3.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		bool write16)
{
  LabelImage3::Pointer image = readImage<LabelImage3>(inputImageName);
  std::map<Label, unsigned int> smap;
  getSizeMap<LabelImage3>(smap, image, false);
  Points3 openSet;
  getPoints(openSet, image, BGVAL);
  while (openSet.size() > 0) {
    Points3 changeSet;
    std::list<Label> changeToLabels;
    for (Points3::iterator pit = openSet.begin(); pit != openSet.end(); 
	 ++pit) {
      std::vector<Pixel3<Label> > n = 
	getNeighbors<LabelImage3>(*pit, image, 4);
      Label l = BGVAL;
      unsigned int a = UINT_MAX;
      for (std::vector<Pixel3<Label> >::const_iterator nit = n.begin(); 
	   nit != n.end(); ++nit) {
	unsigned int thisa = smap[nit->val];
	if (nit->val != BGVAL && thisa < a) {
	  a = thisa;
	  l = nit->val;
	}
      }
      if (l != BGVAL) {
	Points3::iterator tpit = pit;
	--pit;
	changeSet.splice(changeSet.end(), openSet, tpit);
	changeToLabels.push_back(l);
      }
    }
    Points3::const_iterator pit = changeSet.begin();
    std::list<Label>::const_iterator lit = changeToLabels.begin();
    while (pit != changeSet.end()) {
      setv<LabelImage3>(image, *pit, *lit);
      ++pit;
      ++lit;
    }
  }
  if (write16) 
    writeImage<UInt16Image3>(outputImageName, 
			     castImage<LabelImage3, UInt16Image3>(image));
  else writeImage<LabelImage3>(outputImageName, image);
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

