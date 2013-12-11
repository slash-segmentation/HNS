#include "util/io_util.h"
#include "link3d_util.h"
using namespace n3;
using namespace n3::link3d;

void operation (std::vector<const char*> const& outputImageNames, 
		std::vector<const char*> const& inputImageNames, 
		std::vector<const char*> const& regionPairFileNames, 
		bool write16)
{
  int n = regionPairFileNames.size();
  std::map<int, LabelImage::Pointer> inputImages, outputImages;
  std::list<fLink> links;
  for (int i = 0; i < n; ++i) {
    std::list<LabelPair> pairs;
    readPairs(pairs, regionPairFileNames[i], ' ', '\n');
    for (std::list<LabelPair>::const_iterator pit = pairs.begin(); 
	 pit != pairs.end(); ++pit) 
      links.push_back(fLink(i, pit->first, i + 1, pit->second, 0.0));
  }  
  for (int i = 0; i < n + 1; ++i) 
    inputImages[i] = readImage<LabelImage>(inputImageNames[i]);
  link(outputImages, inputImages, links);
  if (write16)
    for (int i = 0; i < n + 1; ++i) {
      UInt16Image::Pointer outImage = 
	castImage<LabelImage, UInt16Image>(outputImages[i]);
      writeImage<UInt16Image>(outputImageNames[i], outImage);
    }
  else 
    for (int i = 0; i < n + 1; ++i)
      writeImage<LabelImage>(outputImageNames[i], outputImages[i]);
}


int main (int argc, char* argv[])
{
  if (argc < 6) {
    std::cerr << "Usage: " << argv[0] 
	      << " {-i inputImageName outputImageName}"
	      << " {-l regionPairFileName}"
	      << " [writeToUInt16 = 0]"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  std::vector<const char*> inputImageNames, outputImageNames;
  inputImageNames.reserve(argc / 5 + 1);
  outputImageNames.reserve(argc / 5 + 1);
  while (strcmp(argv[argi], "-i") == 0) {
    inputImageNames.push_back(argv[argi + 1]);
    outputImageNames.push_back(argv[argi + 2]);
    argi += 3;
  }
  std::vector<const char*> regionPairFileNames;
  regionPairFileNames.reserve(argc / 5 + 1);
  while (strcmp(argv[argi], "-l") == 0) {
    regionPairFileNames.push_back(argv[argi + 1]);
    argi += 2;
  }
  bool write16 = (argi < argc && atoi(argv[argi++]) != 0? true: false);
  operation(outputImageNames, inputImageNames, 
	    regionPairFileNames, write16);
  return EXIT_SUCCESS;
}
