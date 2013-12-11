#include "util/image_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		double inputMin, double inputMax, double outputMin, 
		double outputMax, bool invert)
{
  FloatImage::Pointer im = readImage<FloatImage>(inputImageName);
  double k = (outputMax - outputMin) / (inputMax - inputMin);
  if (invert) 
    for (itk::ImageRegionIterator<FloatImage> 
	   it(im, im->GetLargestPossibleRegion()); !it.IsAtEnd(); ++it)
      it.Set(outputMax - k * (it.Get() - inputMin));
  else 
    for (itk::ImageRegionIterator<FloatImage> 
	   it(im, im->GetLargestPossibleRegion()); !it.IsAtEnd(); ++it)
      it.Set(k * (it.Get() - inputMin) + outputMin);
  writeImage<FloatImage>(outputImageName, im);
}



int main (int argc, char* argv[])
{
  if (argc != 7 && argc != 8) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputImageName" 
	      << " inputMin"
	      << " inputMax" 
	      << " outputMin" 
	      << " outputMax" 
	      << " [invert = 0]"
	      << " outputImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  double inputMin = atof(argv[argi++]);
  double inputMax = atof(argv[argi++]);
  double outputMin = atof(argv[argi++]);
  double outputMax = atof(argv[argi++]);
  bool invert = (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false);
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, inputMin, inputMax, outputMin, 
	    outputMax, invert);
  return EXIT_SUCCESS;
}
