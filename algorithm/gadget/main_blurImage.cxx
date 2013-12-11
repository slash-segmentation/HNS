#include "util/image_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		double sigma, int kernelWidth, bool rescale, 
		Float outputMin, Float outputMax)
{
  FloatImage::Pointer inputImage = readImage<FloatImage>(inputImageName);
  FloatImage::Pointer outputImage = 
    blurImage<FloatImage>(inputImage, sigma, kernelWidth);
  if (rescale) 
    outputImage = 
      rescaleImage<FloatImage>(outputImage, outputMin, outputMax);
  writeImage<FloatImage>(outputImageName, outputImage);
}



int main (int argc, char* argv[])
{
  if (argc < 3 || argc > 8) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputImageName" 
	      << " [sigma = 1.0]" 
	      << " [kernelWidth = (int)(6 * sigma + 1)]"
	      << " [rescale = 0]"
	      << " [outputMin = 0]"
	      << " [outputMax = 1]"
	      << " outputImageName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  double sigma = (argi < argc - 1? atof(argv[argi++]): 1.0);
  int kernelWidth = (argi < argc - 1? atoi(argv[argi++]): 
		     (int)(6.0 * sigma + 1.0));
  bool rescale = (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false);
  Float outputMin = (argi < argc - 1? atof(argv[argi++]): 0.0);
  Float outputMax = (argi < argc - 1? atof(argv[argi++]): 1.0);
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, sigma, kernelWidth, 
	    rescale, outputMin, outputMax);
  return EXIT_SUCCESS;
}
