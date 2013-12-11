#include "util/image_util.h"
using namespace n3;

void operation (const char* resImageName, const char* valImageName, 
		Float seedThreshold, bool keepWatershedLine, 
		bool isFullyConnected, bool write16)
{
  FloatImage::Pointer valImage = readImage<FloatImage>(valImageName);
  if (write16) {
    UInt16Image::Pointer resImage = 
      watershed<FloatImage, UInt16Image>(valImage, seedThreshold, 
					 keepWatershedLine, 
					 isFullyConnected);
    writeImage<UInt16Image>(resImageName, resImage);
  }
  else {
    LabelImage::Pointer resImage = 
      watershed<FloatImage, LabelImage>(valImage, seedThreshold, 
					keepWatershedLine, 
					isFullyConnected);
    writeImage<LabelImage>(resImageName, resImage);
  }  
}



int main (int argc, char* argv[])
{
  if (argc < 4 || argc > 7) {
    std::cerr << "Usage: " << argv[0] 
	      << " boundaryValueImageName" 
	      << " seedThreshold" 
	      << " [writeToUInt16Image = 0]"
	      << " [keepWatershedLine = 1]"
	      << " [isBoundaryFullyConnected = 1]"
	      << " resultImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* valImageName = argv[argi++];
  Float seedThreshold = atof(argv[argi++]);
  bool write16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? 
		  true: false);
  bool keepWatershedLine = (argi < argc - 1 && atoi(argv[argi++]) == 0? 
			    false: true);
  bool isFullyConnected = (argi < argc - 1 && atoi(argv[argi++]) == 0? 
			   false: true);
  const char* resImageName = argv[argi++];
  operation(resImageName, valImageName, seedThreshold, keepWatershedLine, 
	    isFullyConnected, write16);
  return EXIT_SUCCESS;
}
