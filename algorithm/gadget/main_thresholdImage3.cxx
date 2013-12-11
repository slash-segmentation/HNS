#include "util/image_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		double lowerThreshold, double upperThreshold, 
		Label insideValue, Label outsideValue, bool labelCC, 
		Label bgValue, bool write16)
{
  FloatImage3::Pointer inputImage = readImage<FloatImage3>(inputImageName);
  if (write16) {
    UInt16Image3::Pointer resImage = 
      thresholdImage<FloatImage3, UInt16Image3>
      (inputImage, lowerThreshold, upperThreshold, 
       insideValue, outsideValue);
    if (labelCC) 
      resImage = 
	labelConnectedComponents<UInt16Image3, UInt16Image3>
	(resImage, bgValue, false, true, 0);
    writeImage<UInt16Image3>(outputImageName, resImage);
  }
  else {
    LabelImage3::Pointer resImage = 
      thresholdImage<FloatImage3, LabelImage3>
      (inputImage, lowerThreshold, upperThreshold, 
       insideValue, outsideValue);
    if (labelCC) 
      resImage = 
	labelConnectedComponents<LabelImage3, LabelImage3>
	(resImage, bgValue, false, true, 0);
    writeImage<LabelImage3>(outputImageName, resImage);
  }
}



int main (int argc, char* argv[])
{
  if (argc < 5 || argc > 10) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputImageName"
	      << " lowerThreshold"
	      << " upperThreshold"
	      << " [labelConnectedComponents = 0]"
	      << " [writeToUInt16Image = 0]"
	      << " [insideValue = 1]"
	      << " [outsideValue = 0]"
	      << " [bgValue = outsideValue]"
	      << " outputImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  double lowerThreshold = atof(argv[argi++]);
  double upperThreshold = atof(argv[argi++]);
  bool labelCC = argi < argc - 1 && atoi(argv[argi++]) != 0? true: false;
  bool write16 = argi < argc - 1 && atoi(argv[argi++]) != 0? true: false;
  Label insideValue = argi < argc - 1? atol(argv[argi++]): 1;
  Label outsideValue = argi < argc - 1? atol(argv[argi++]): 0;
  Label bgValue = argi < argc - 1? atol(argv[argi++]): outsideValue;
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, lowerThreshold, 
	    upperThreshold, insideValue, outsideValue, labelCC, 
	    bgValue, write16);
  return EXIT_SUCCESS;
}
