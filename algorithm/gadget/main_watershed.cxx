#include "util/image_util.h"
using namespace n3;

void operation (const char* resImageName, const char* valImageName, 
		Float seedThreshold, bool keepWatershedLine, 
		bool isFullyConnected, bool write16)
{
  FloatImage::Pointer valImage = readImage<FloatImage>(valImageName);
  if (write16) {
    UInt16Image::Pointer resImage = 
      watershed<FloatImage, UInt16Image>(valImage, seedThreshold, 
					 keepWatershedLine, 
					 isFullyConnected);
    writeImage<UInt16Image>(resImageName, resImage);
  }
  else {
    LabelImage::Pointer resImage = 
      watershed<FloatImage, LabelImage>(valImage, seedThreshold, 
					keepWatershedLine, 
					isFullyConnected);
    writeImage<LabelImage>(resImageName, resImage);
  }  
}



Float get_seed_threshold(char* s)
{
  Float f;
  FILE *F;
  if (strlen(s) == 1 && *s == '-') { F = stdin; }
  else {
    char* e;
    f = strtod(s, &e);
    if (*e == 0) { return f; }
    F = fopen(s, "r");
    if (F == NULL) { return -1.0; }
  }
  int c = fscanf(F, "%f", &f);
  if (F != stdin) { fclose(F); }
  return (c <= 0) ? -1.0 : f;
}



int main (int argc, char* argv[])
{
  if (argc < 4 || argc > 7) {
    std::cerr << "Usage: " << argv[0] 
	      << " boundaryValueImageName" 
	      << " seedThreshold" 
	      << " [writeToUInt16Image = 0]"
	      << " [keepWatershedLine = 1]"
	      << " [isBoundaryFullyConnected = 1]"
	      << " resultImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* valImageName = argv[argi++];
  Float seedThreshold = get_seed_threshold(argv[argi++]);
  if (seedThreshold <= 0.0) {
    std::cerr << "Seed theshold must be a positive floating-point value, a path of a file containing a positive floating-point value, or - to read the value from stdin." << std::endl;
    return EXIT_FAILURE;
  }
  bool write16 = (argi < argc - 1 && atoi(argv[argi++]) != 0);
  bool keepWatershedLine = (argi >= argc - 1 || atoi(argv[argi++]) != 0);
  bool isFullyConnected = (argi >= argc - 1 || atoi(argv[argi++]) != 0);
  const char* resImageName = argv[argi++];
  operation(resImageName, valImageName, seedThreshold, keepWatershedLine, 
	    isFullyConnected, write16);
  return EXIT_SUCCESS;
}
