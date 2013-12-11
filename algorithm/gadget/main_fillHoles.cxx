#include "util/image_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		bool write16)
{
  LabelImage::Pointer image = readImage<LabelImage>(inputImageName);
  if (write16) {
    UInt16Image::Pointer out = 
      fillHoles<LabelImage, UInt16Image>(image, false);
    writeImage<UInt16Image>(outputImageName, out);
  }
  else {
    image = fillHoles<LabelImage, LabelImage>(image, false);
    writeImage<LabelImage>(outputImageName, image);
  }
}



int main (int argc, char* argv[])
{
  if (argc != 3 && argc != 4) {
    std::cerr << "Usage: " << argv[0]
	      << " inputImageName"
	      << " [writeToUInt16 = 0]"
	      << " outputImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  bool write16 = argi < argc - 1 && atoi(argv[argi++]) != 0? true: false;
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, write16);
  return EXIT_SUCCESS;
}
