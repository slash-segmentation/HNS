#include "util/image_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		Float max)
{
  FloatImage::Pointer inputImage = readImage<FloatImage>(inputImageName);
  FloatImage::Pointer outputImage = 
    invertImage<FloatImage>(inputImage, max);
  writeImage<FloatImage>(outputImageName, outputImage);
}



int main (int argc, char* argv[])
{
  if (argc != 3 && argc != 4) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputImageName" 
	      << " [maximum = 1.0]" 
	      << " outputImageName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  Float max = (argi < argc - 1? atof(argv[argi++]): 1.0);
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, max);
  return EXIT_SUCCESS;
}
