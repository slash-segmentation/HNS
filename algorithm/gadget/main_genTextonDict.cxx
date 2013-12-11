#include "util/cv/cv_texton.h"
using namespace n3;

void operation (const char* tdictFileName, 
		std::vector<const char*> const& valImageNames, 
		std::vector<const char*> const& labelImageNames, 
		int maxIterationNum, double convergenceThreshold)
{
  std::list<TextonPatch> pats;
  int n = valImageNames.size();
  LabelImage::Pointer canvas = readImage<LabelImage>(labelImageNames[0]);
  for (int i = 0; i < n; ++i) {
    std::cerr << "Generating texton patches for slice " << i << "... ";
    FloatImage::Pointer valImage = 
      readImage<FloatImage>(valImageNames[i]);
    LabelImage::Pointer labelImage = 
      readImage<LabelImage>(labelImageNames[i]);
    PointMap rmap;
    getPointMap(rmap, labelImage, false);
    for (PointMap::const_iterator it = rmap.begin(); it != rmap.end(); 
	 ++it) genTextonPatch(pats, valImage, it->second, canvas, true);
    std::cerr << "Done." << std::endl;
  }
  TextonDict tdict;
  genTextonDict(tdict, pats, maxIterationNum, convergenceThreshold, 
		TEXTON_PAT_LEN);
  writeTexton(tdictFileName, tdict);
}



int main (int argc, char* argv[])
{
  if (argc < 6) {
    std::cerr << "Usage: " << argv[0] 
	      << " {-v valueImageName}"
	      << " {-l labelImageName}"
	      << " [maxIterationNum = 10000]"
	      << " [convergenceThreshold = 2e-4]"
	      << " textonDictionaryFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  std::vector<const char*> valImageNames, labelImageNames;
  valImageNames.reserve(argc - 2);
  labelImageNames.reserve(argc - 2);
  while (argi < argc - 1) {
    if (strcmp(argv[argi], "-v") == 0) {
      valImageNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else if (strcmp(argv[argi], "-l") == 0) {
      labelImageNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else break;
  }
  int maxIterationNum = (argi < argc - 1? atoi(argv[argi++]): 10000);
  double convergenceThreshold = (argi < argc - 1? atof(argv[argi++]): 2e-4);
  const char* tdictFileName = argv[argi++];  
  operation(tdictFileName, valImageNames, labelImageNames, 
	    maxIterationNum, convergenceThreshold);
  return EXIT_SUCCESS;
}
