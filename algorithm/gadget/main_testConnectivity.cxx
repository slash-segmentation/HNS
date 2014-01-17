#include "util/image_util.h"
using namespace n3;

void operation (const char* labelImageName) 
{
  float d = (float)sqrt(3.0f);
  LabelImage3::Pointer im = readImage<LabelImage3>(labelImageName);
  int n = 0;
  for (itk::ImageRegionConstIteratorWithIndex<LabelImage3> 
	 iit(im, im->GetLargestPossibleRegion()); !iit.IsAtEnd(); ++iit)
    if (iit.Get() == 1) {
      Point3 p = i2p(iit.GetIndex());
      Points3 nb;
      getNeighbors<LabelImage3>(nb, p, im, 26, 1);
      if (nb.size() > 1) {
	bool allConnected = true;
	for (Points3::const_iterator nit0 = nb.begin(); nit0 != nb.end(); 
	     ++nit0) {
	  Points3::const_iterator nit1 = nit0; ++nit1;
	  while (nit1 != nb.end()) {
	    if (nit0->get_distance(*nit1) > d) {
	      allConnected = false;
	      break;
	    }
	    ++nit1;
	  }
	  if (!allConnected) break;
	}
	if (allConnected) {
	  std::cout << n++ << "\n" << p << "\n";
	  for (Points3::const_iterator nit = nb.begin(); nit != nb.end(); 
	       ++nit) std::cout << *nit << "\n";
	}
      }
    }
}



int main (int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0]
	      << " labelImageName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  operation(labelImageName);
  return EXIT_SUCCESS;
}
