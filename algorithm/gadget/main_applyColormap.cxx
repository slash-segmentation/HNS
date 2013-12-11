#include "util/vis_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* scalarImageName, 
		const char* pixelType, const char* colormapName, 
		bool useExtremaForScaling, const char* bgFillColor)
{
  Rgb8Image::Pointer outputImage;
  if (strcmp(pixelType, "float") == 0) {
    FloatImage::Pointer scalarImage = 
      readImage<FloatImage>(scalarImageName);
    if (colormapName != NULL)
      outputImage = 
	applyColormap<FloatImage>(scalarImage, 
				  colormap<FloatImage>(colormapName), 
				  useExtremaForScaling);
    else outputImage = randomColor<FloatImage>(scalarImage, bgFillColor);
  }
  else if (strcmp(pixelType, "uint32") == 0) {
    UInt32Image::Pointer scalarImage = 
      readImage<UInt32Image>(scalarImageName);
    if (colormapName != NULL)
      outputImage = 
	applyColormap<UInt32Image>(scalarImage, 
				   colormap<UInt32Image>(colormapName), 
				   useExtremaForScaling);
    else outputImage = randomColor<UInt32Image>(scalarImage, bgFillColor);
  }
  else if (strcmp(pixelType, "uint16") == 0) {
    UInt16Image::Pointer scalarImage = 
      readImage<UInt16Image>(scalarImageName);
    if (colormapName != NULL)
      outputImage = 
	applyColormap<UInt16Image>(scalarImage, 
				   colormap<UInt16Image>(colormapName), 
				   useExtremaForScaling);
    else outputImage = randomColor<UInt16Image>(scalarImage, bgFillColor);
  }
  else if (strcmp(pixelType, "uint8") == 0) {
    UInt8Image::Pointer scalarImage = 
      readImage<UInt8Image>(scalarImageName);
    if (colormapName != NULL)
      outputImage = 
	applyColormap<UInt8Image>(scalarImage, 
				  colormap<UInt8Image>(colormapName), 
				  useExtremaForScaling);
    else outputImage = randomColor<UInt8Image>(scalarImage, bgFillColor);
  }
  else perr("Error: unsupported scalar image pixel type...");
  writeImage<Rgb8Image>(outputImageName, outputImage);
}



int main (int argc, char* argv[])
{
  if (argc < 5 || argc > 7) {
    std::cerr << "Usage: " << argv[0] 
	      << " scalarImageName"
	      << " scalarImagePixelType ('float' or 'uint*')"
	      << " colormapName (all lowercase ITK or 'NULL' for random)"
	      << " [useInputExtremaForScaling = 1 (only for non-random)]"
	      << " [bgFillColor = 'NULL' (only for random, 'NULL' to skip)]"
	      << " outputImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* scalarImageName = argv[argi++];
  const char* pixelType = argv[argi++];
  const char* colormapName = 
    strcmp(argv[argi], "NULL") == 0? NULL: argv[argi];
  ++argi;
  bool useExtremaForScaling = 
    argi < argc - 1 && atoi(argv[argi++]) != 0? true: false;
  const char* bgFillColor = NULL;
  if (argi < argc - 1) {
    if (strcmp(argv[argi], "NULL") != 0) bgFillColor = argv[argi];
    ++argi;
  }
  const char* outputImageName = argv[argi++];
  operation(outputImageName, scalarImageName, pixelType, 
	    colormapName, useExtremaForScaling, bgFillColor);
  return EXIT_SUCCESS;
}
