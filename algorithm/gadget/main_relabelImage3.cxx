#include "util/image_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		bool isWriteUInt16, int minRegionArea)
{
  LabelImage3::Pointer inputImage = readImage<LabelImage3>(inputImageName);
  if (isWriteUInt16) {
    UInt16Image3::Pointer outputImage = 
      relabelImage<LabelImage3, UInt16Image3>(inputImage, minRegionArea);
    writeImage<UInt16Image3>(outputImageName, outputImage);
  }
  else {
    LabelImage3::Pointer outputImage = 
      relabelImage<LabelImage3, LabelImage3>(inputImage, minRegionArea);
    writeImage<LabelImage3>(outputImageName, outputImage);
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
