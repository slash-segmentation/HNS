#include "util/image_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		double sigma, int kernelWidth, bool rescale, 
		Float outputMin, Float outputMax, bool padBorders)
{
  FloatImage3::Pointer inputImage = readImage<FloatImage3>(inputImageName);
  ImageRegion3 oreg = inputImage->GetLargestPossibleRegion();
  FloatImage3::Pointer outputImage;
  if (padBorders) {
    int w = kernelWidth / 2;
    inputImage = 
      padImage<FloatImage3, FloatImage3>
      (inputImage, getImageSize(w, w, w), getImageSize(w, w, w));
    outputImage = blurImage<FloatImage3>(inputImage, sigma, kernelWidth);
    outputImage = cropImage<FloatImage3, FloatImage3>(outputImage, oreg);
  }
  else 
    outputImage = blurImage<FloatImage3>(inputImage, sigma, kernelWidth);
  if (rescale) 
    outputImage = 
      rescaleImage<FloatImage3>(outputImage, outputMin, outputMax);
  writeImage<FloatImage3>(outputImageName, outputImage);
}



int main (int argc, char* argv[])
{
  if (argc < 3 || argc > 9) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputImageName" 
	      << " [sigma = 1.0]" 
	      << " [kernelWidth = (int)(6 * sigma + 1)]"
	      << " [rescale = 0]"
	      << " [outputMin = 0]"
	      << " [outputMax = 1]"
	      << " [padBorders = 1]"
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
  bool padBorders = argi >= argc - 1 || atoi(argv[argi++]) != 0;
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, sigma, kernelWidth, 
	    rescale, outputMin, outputMax, padBorders);
  return EXIT_SUCCESS;
}
