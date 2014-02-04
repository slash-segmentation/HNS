#include "util/cv/cv_ma.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		Label label, bool is2D, bool write16) 
{
  if (is2D) {
    LabelImage::Pointer im = readImage<LabelImage>(inputImageName);
    LabelImage::Pointer canvas = createImage<LabelImage>(im, 0);
    itk::ImageRegionConstIterator<LabelImage> 
      iit0(im, im->GetLargestPossibleRegion());
    itk::ImageRegionIterator<LabelImage> 
      iit1(canvas, canvas->GetLargestPossibleRegion());
    while (!iit0.IsAtEnd()) {
      if (iit0.Get() == label) iit1.Set(1);
      ++iit0;
      ++iit1;
    }
    LabelImage::Pointer resImage = binaryThin(canvas, NULL);
    if (write16) 
      writeImage<UInt16Image>(outputImageName, castImage<LabelImage, 
			      UInt16Image>(resImage));
    else writeImage<LabelImage>(outputImageName, resImage);
  }
  else {
    LabelImage3::Pointer im = readImage<LabelImage3>(inputImageName);
    LabelImage3::Pointer canvas = createImage<LabelImage3>(im, 0);
    itk::ImageRegionConstIterator<LabelImage3> 
      iit0(im, im->GetLargestPossibleRegion());
    itk::ImageRegionIterator<LabelImage3> 
      iit1(canvas, canvas->GetLargestPossibleRegion());
    while (!iit0.IsAtEnd()) {
      if (iit0.Get() == label) iit1.Set(1);
      ++iit0;
      ++iit1;
    }
    LabelImage3::Pointer resImage = binaryThin(canvas, NULL);
    if (write16) 
      writeImage<UInt16Image3>(outputImageName, castImage<LabelImage3, 
			       UInt16Image3>(resImage));
    else writeImage<LabelImage3>(outputImageName, resImage);
  }
}



int main (int argc, char* argv[])
{
  if (argc < 5 || argc > 6) {
    std::cerr << "Usage: " << argv[0] 
    	      << " inputImageName"
    	      << " regionLabel"
    	      << " is2D (0 for 3D)"
    	      << " [writeToUInt16 = 1]"
    	      << " outputImageName"
    	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  Label label = atol(argv[argi++]);
  bool is2D = atoi(argv[argi++]) == 0? false: true;
  bool write16 = argi < argc - 1 && atoi(argv[argi++]) == 0? false: true;
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, label, is2D, write16);
  return EXIT_SUCCESS;
}
