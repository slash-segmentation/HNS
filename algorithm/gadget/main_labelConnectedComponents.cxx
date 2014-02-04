#include "util/image_util.h"
using namespace n3;

// // If more than one input images, label 3d connected components
// void operation (std::vector<const char*> const& outputImageNames, 
// 		std::vector<const char*> const& inputImageNames, 
// 		bool write16, Label bgVal, bool isFullyConnected)
// {
//   std::vector<LabelImage::Pointer> inputImages, outputImages;
//   inputImages.reserve(inputImageNames.size());
//   for (std::vector<const char*>::const_iterator iit = 
// 	 inputImageNames.begin(); iit != inputImageNames.end(); ++iit) 
//     inputImages.push_back(readImage<LabelImage>(*iit));
//   if (inputImages.size() == 1) {
//     outputImages.reserve(1);    
//     outputImages.push_back(labelConnectedComponents<LabelImage, LabelImage>
// 			   (inputImages[0], bgVal, isFullyConnected));
//   }
//   else {
//     LabelImage3::Pointer inputVol = 
//       stackImages<LabelImage, LabelImage3>(inputImages);
//     LabelImage3::Pointer outputVol = 
//       labelConnectedComponents<LabelImage3, LabelImage3>(inputVol, bgVal, 
// 							 isFullyConnected, 
// 							 false, 0);
//     unstackImages<LabelImage3, LabelImage>(outputImages, outputVol);
//   }
//   int n = outputImages.size();
//   for (int i = 0; i < n; ++i)
//     if (write16) {
//       UInt16Image::Pointer out = 
// 	castImage<LabelImage, UInt16Image>(outputImages[i]);
//       writeImage<UInt16Image>(outputImageNames[i], out);
//     }
//     else writeImage<LabelImage>(outputImageNames[i], outputImages[i]);
// }

// int main (int argc, char* argv[])
// {
//   if (argc < 4) {
//     std::cerr << "Usage: " << argv[0] << " (3d CC if more than one image)"
// 	      << " {-i inputImageName outputImageName}"
// 	      << " [writeToUInt16 = 0]"
// 	      << " [backgroundValue = BGVAL]"
// 	      << " [isFullyConnected = 0]"
// 	      << std::endl;
//     return EXIT_FAILURE;
//   }
//   int argi = 1;
//   std::vector<const char*> inputImageNames, outputImageNames;
//   inputImageNames.reserve((argc - 1) / 3);
//   outputImageNames.reserve((argc - 1) / 3);
//   while (strcmp(argv[argi], "-i") == 0) {
//     ++argi;
//     inputImageNames.push_back(argv[argi++]);
//     outputImageNames.push_back(argv[argi++]);
//   }
//   bool write16 =  argi < argc && atoi(argv[argi++]) == 1? true: false;
//   Label bgVal = argi < argc? atol(argv[argi++]): BGVAL;
//   bool isFullyConnected = argi < argc && atoi(argv[argi++]) == 1? 
// 				 true: false;
//   operation(outputImageNames, inputImageNames, write16, bgVal, 
// 	    isFullyConnected);
//   return EXIT_SUCCESS;
// }


void operation (std::vector<const char*> const& outputImageNames, 
		std::vector<const char*> const& inputImageNames, 
		bool is2D, bool write16, Label bgVal, bool isFullyConnected)
{
  if (is2D) { // 2D CC
    std::vector<const char*>::const_iterator iit = inputImageNames.begin();
    std::vector<const char*>::const_iterator oit = outputImageNames.begin();
    while (iit != inputImageNames.end()) {
      LabelImage::Pointer inputImage = readImage<LabelImage>(*iit);
      LabelImage::Pointer resImage = 
	labelConnectedComponents<LabelImage, LabelImage>
	(inputImage, bgVal, isFullyConnected, true, 0);
      if (write16)
	writeImage<UInt16Image>(*oit, relabelImage<LabelImage, 
				UInt16Image>(resImage));
      else 
	writeImage<LabelImage>(*oit, relabelImage<LabelImage, 
			       LabelImage>(resImage));
      ++iit;
      ++oit;
    }
  }
  else { // 3D CC
    LabelImage3::Pointer inputImage;
    if (inputImageNames.size() == 1) 
      inputImage = readImage<LabelImage3>(inputImageNames[0]);
    else inputImage = readImage<LabelImage3>(inputImageNames);
    LabelImage3::Pointer resImage = 
      labelConnectedComponents<LabelImage3, LabelImage3>
      (inputImage, bgVal, isFullyConnected, true, 0);
    if (write16) {
      if (outputImageNames.size() == 1)
	writeImage<UInt16Image3>(outputImageNames[0], 
				 relabelImage<LabelImage3, 
				 UInt16Image3>(resImage));
      else 
	writeImage<UInt16Image3, UInt16Image>(outputImageNames, 
					      relabelImage<LabelImage3, 
					      UInt16Image3>(resImage));
    }
    else {
      if (outputImageNames.size() == 1)
	writeImage<LabelImage3>(outputImageNames[0], 
				relabelImage<LabelImage3, 
				LabelImage3>(resImage));
      else 
	writeImage<LabelImage3, LabelImage>(outputImageNames, 
					    relabelImage<LabelImage3, 
					    LabelImage3>(resImage));
    }
  }
}



int main (int argc, char* argv[])
{
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] 
	      << " {-i inputImageName}"
	      << " {-o outputImageName}"
	      << " isLabelConnectedComponentsIn2D (0 for 3D)"
	      << " [writeToUInt16 = 0]" 
	      << " [backgroundValue = BGVAL]"
	      << " [isFullyConnected = 0]" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  std::vector<const char*> inputImageNames, outputImageNames;
  inputImageNames.reserve(argc - 2);
  outputImageNames.reserve(argc - 2);
  bool isi = strcmp(argv[argi], "-i") == 0, 
    iso = strcmp(argv[argi], "-o") == 0;
  while (isi || iso) {
    ++argi;
    if (isi) inputImageNames.push_back(argv[argi++]);
    else if (iso) outputImageNames.push_back(argv[argi++]);
    isi = strcmp(argv[argi], "-i") == 0; 
    iso = strcmp(argv[argi], "-o") == 0;
  }
  bool is2D = atoi(argv[argi++]) == 0? false: true;
  bool write16 =  argi < argc && atoi(argv[argi++]) == 1? true: false;
  Label bgVal = argi < argc? atol(argv[argi++]): BGVAL;
  bool isFullyConnected = argi < argc && atoi(argv[argi++]) == 1? 
  				 true: false;
  operation(outputImageNames, inputImageNames, is2D, write16, bgVal, 
  	    isFullyConnected); 
  return EXIT_SUCCESS;
}
