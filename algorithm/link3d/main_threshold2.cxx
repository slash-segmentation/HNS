#include "link3d_util.h"
#include "link3d_io.h"
using namespace n3;
using namespace n3::link3d;

void operation (std::map<int, const char*> const& outputImageNames, 
		std::map<int, const char*> const& inputImageNames, 
		const char* bodyPairFileName, const char* weightFileName, 
		double wth, bool write16)
{
  std::map<int, LabelImage::Pointer> outputImages, inputImages;
  for (std::map<int, const char*>::const_iterator nit = 
	 inputImageNames.begin(); nit != inputImageNames.end(); ++nit) 
    inputImages.insert(inputImages.end(), 
		       std::make_pair(nit->first, 
				      readImage<LabelImage>(nit->second)));
  std::list<fBodyLink> links, picks;
  readBodyLinks(links, bodyPairFileName, weightFileName);
  for (std::list<fBodyLink>::const_iterator lit = links.begin(); 
       lit != links.end(); ++lit) 
    if (lit->data >= wth) picks.push_back(*lit);
  linkBodies(outputImages, inputImages, picks);
  if (write16)
    for (std::map<int, const char*>::const_iterator it = 
	   outputImageNames.begin(); it != outputImageNames.end(); ++it) {
      UInt16Image::Pointer outImage = 
	castImage<LabelImage, UInt16Image>(outputImages[it->first]);
      writeImage<UInt16Image>(it->second, outImage);
    }
  else 
    for (std::map<int, const char*>::const_iterator it = 
	   outputImageNames.begin(); it != outputImageNames.end(); ++it)
      writeImage<LabelImage>(it->second, outputImages[it->first]);
}



int main (int argc, char* argv[])
{
  if (argc < 8) {
    std::cerr << "Usage: " << argv[0] 
	      << " {-i sliceNo inputImageName outputImageName}"
	      << " bodyPairFileName"
	      << " weightFileName"
	      << " threshold"
	      << " [writeToUInt16 = 0]"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  std::map<int, const char*> inputImageNames, outputImageNames;
  while (strcmp(argv[argi], "-i") == 0) {
    ++argi;
    int sliceNo = atoi(argv[argi++]);
    inputImageNames[sliceNo] = argv[argi++];
    outputImageNames[sliceNo] = argv[argi++];    
  }
  const char* bodyPairFileName = argv[argi++];
  const char* weightFileName = argv[argi++];
  double wth = atof(argv[argi++]);
  bool write16 = (argi < argc && atoi(argv[argi++]) != 0? true: false);
  operation(outputImageNames, inputImageNames, bodyPairFileName, 
	    weightFileName, wth, write16);
  return EXIT_SUCCESS;
}
