#include "util/image_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		bool isWriteUInt16, int minRegionArea)
{
  LabelImage::Pointer inputImage = readImage<LabelImage>(inputImageName);
  if (isWriteUInt16) {
    UInt16Image::Pointer outputImage = 
      relabelImage<LabelImage, UInt16Image>(inputImage, minRegionArea);
    writeImage<UInt16Image>(outputImageName, outputImage);
  }
  else {
    LabelImage::Pointer outputImage = 
      relabelImage<LabelImage, LabelImage>(inputImage, minRegionArea);
    writeImage<LabelImage>(outputImageName, outputImage);
  }
}



int main (int argc, char* argv[])
{
  if (argc != 4 && argc != 5) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputImageName" 
	      << " isWriteToUInt16Image"
	      << " [minRegionArea = 0]"
	      << " outputImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  bool isWriteUInt16 = (atoi(argv[argi++]) == 0? false: true);
  int minRegionArea = (argi < argc - 1? atoi(argv[argi++]): 0);
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, isWriteUInt16, 
	    minRegionArea);
  return EXIT_SUCCESS;
}
