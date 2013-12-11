#include "util/image_util.h"
#include "n3_io.h"
using namespace n3;

void operation (const char* outputFileName, const char* inputImageName, 
		const char* type, bool writeToUInt16)
{
  LabelImage::Pointer image = readImage<LabelImage>(inputImageName);
  if (strcmp(type, "image") == 0) 
    if (writeToUInt16) {
      UInt16Image::Pointer output = 
	castImage<LabelImage, UInt16Image>(image);
      writeImage<UInt16Image>(outputFileName, output);
    }
    else writeImage<LabelImage>(outputFileName, image);  
  else if (strcmp(type, "text") == 0) {
    int width = getw<LabelImage>(image);
    int height = geth<LabelImage>(image);
    std::vector<Label> matrix(width * height);
    for (itk::ImageRegionConstIteratorWithIndex<LabelImage> 
	   it(image, image->GetLargestPossibleRegion()); !it.IsAtEnd(); 
	 ++it) 
      matrix[width * it.GetIndex()[1] + it.GetIndex()[0]] = it.Get();
    write(outputFileName, matrix, width, ' ');
  }
  else perr("Error: unsupported type...");
}



int main (int argc, char* argv[])
{
  if (argc != 4 && argc != 5) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputImageName" 
	      << " outputType ('image' or 'text')"
	      << " [writeToUInt16Image = 0]"
	      << " outputFileName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  const char* type = argv[argi++];
  bool writeToUInt16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? 
			true: false);
  const char* outputFileName = argv[argi++];
  operation(outputFileName, inputImageName, type, writeToUInt16);
}
