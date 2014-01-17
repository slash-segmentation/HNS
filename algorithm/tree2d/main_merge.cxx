#include "tree2d_util.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* outputImageName, const char* inputImageName, 
		const char* pbImageName, int ath0, int ath1, double pth, 
		bool write16, bool relabel)
{
  LabelImage::Pointer labelImage = readImage<LabelImage>(inputImageName);
  labelImage = relabelImage<LabelImage, LabelImage>(labelImage, 0);
  FloatImage::Pointer pbImage = readImage<FloatImage>(pbImageName);
  labelImage = merge(labelImage, pbImage, ath0, ath1, pth);
  if (write16) {
    UInt16Image::Pointer outputImage = 
      (relabel? relabelImage<LabelImage, UInt16Image>(labelImage, 0): 
       castImage<LabelImage, UInt16Image>(labelImage));
    writeImage<UInt16Image>(outputImageName, outputImage);
  }
  else {
    if (relabel) 
      labelImage = relabelImage<LabelImage, LabelImage>(labelImage, 0);
    writeImage<LabelImage>(outputImageName, labelImage);
  }
}



int main (int argc, char* argv[])
{
  if (argc < 7 || argc > 9) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputSegImageName" 
	      << " boundaryProbabilityImageName" 
	      << " areaThreshold0"
	      << " areaThreshold1" 
	      << " probabilityThreshold"
	      << " [writeToUInt16 = 0]"
	      << " [consecutiveOutputLabels = 1]"
	      << " outputSegImageName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  const char* pbImageName = argv[argi++];
  int ath0 = atoi(argv[argi++]);
  int ath1 = atoi(argv[argi++]);
  double pth = atof(argv[argi++]);
  bool write16 = 
    (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false);
  bool relabel = 
    (argi < argc - 1 && atoi(argv[argi++]) == 0? false: true);
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, pbImageName, 
	    ath0, ath1, pth, write16, relabel);
  return EXIT_SUCCESS;
}
