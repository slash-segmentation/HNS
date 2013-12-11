#include "util/image_util.h"
using namespace n3;

void operation (std::vector<const char*> const& resImageNames, 
		std::vector<const char*> const& valImageNames, 
		Float seedThreshold, bool keepWatershedLine, 
		bool isFullyConnected, bool write16)
{
  FloatImage3::Pointer valImage;
  if (valImageNames.size() == 1) 
    valImage = readImage<FloatImage3>(valImageNames[0]);
  else valImage = readImage<FloatImage3>(valImageNames);
  if (write16) {
    UInt16Image3::Pointer resImage = 
      watershed<FloatImage3, UInt16Image3>
      (valImage, seedThreshold, keepWatershedLine, isFullyConnected);
    if (resImageNames.size() == 1) 
      writeImage<UInt16Image3>(resImageNames[0], resImage);
    else writeImage<UInt16Image3, UInt16Image>(resImageNames, resImage);
  }
  else {
    LabelImage3::Pointer resImage = 
      watershed<FloatImage3, LabelImage3>
      (valImage, seedThreshold, keepWatershedLine, isFullyConnected);
    if (resImageNames.size() == 1) 
      writeImage<LabelImage3>(resImageNames[0], resImage);
    else writeImage<LabelImage3, LabelImage>(resImageNames, resImage);
  }
}


int main (int argc, char* argv[])
{
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] 
	      << " {-v boundaryValueImageName}" 
	      << " {-r resultImageName}"
	      << " seedThreshold"
	      << " [writeToUInt16Image = 0]"
	      << " [keepWatershedLine = 1]"
	      << " [isBoundaryFullyConnected = 1]"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  std::vector<const char*> valImageNames, resImageNames;
  valImageNames.reserve((argc - 2) / 4);
  resImageNames.reserve((argc - 2) / 4);
  while (strcmp(argv[argi], "-v") == 0 || strcmp(argv[argi], "-r") == 0) {
    if (strcmp(argv[argi], "-v") == 0) 
      valImageNames.push_back(argv[argi + 1]);
    else if (strcmp(argv[argi], "-r") == 0) 
      resImageNames.push_back(argv[argi + 1]);
    argi += 2;
  }
  Float seedThreshold = atof(argv[argi++]);
  bool write16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? 
		  true: false);
  bool keepWatershedLine = (argi < argc - 1 && atoi(argv[argi++]) == 0? 
			    false: true);
  bool isFullyConnected = (argi < argc - 1 && atoi(argv[argi++]) == 0? 
			   false: true);
  operation(resImageNames, valImageNames, seedThreshold, keepWatershedLine, 
	    isFullyConnected, write16);
  return EXIT_SUCCESS;
}
