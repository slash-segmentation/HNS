#include "util/vis_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* labelImageName, 
		const char* bgImageName, double opacity, 
		const char* bgFillColor)
{
  LabelImage::Pointer labelImage = readImage<LabelImage>(labelImageName);
  UInt8Image::Pointer bgImage = readImage<UInt8Image>(bgImageName);
  Rgb8Image::Pointer outputImage = overlay(labelImage, bgImage, opacity, 
					   bgFillColor);
  writeImage<Rgb8Image>(outputImageName, outputImage);
}



int main (int argc, char* argv[])
{
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0]
	      << " labelImageName" 
	      << " backgroundImageName (in uint8)"
	      << " opacity"
	      << " bgFillColor (e.g. 'black'; 'NULL' to skip)"
	      << " outputImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* bgImageName = argv[argi++];
  double opacity = atof(argv[argi++]);
  const char* bgFillColor = argv[argi++];
  const char* outputImageName = argv[argi++];
  operation(outputImageName, labelImageName, bgImageName, opacity, 
	    bgFillColor);
  return EXIT_SUCCESS;
}
