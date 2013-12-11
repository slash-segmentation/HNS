#include "util/metric/metric_entropy.h"
using namespace n3;

void operation (std::vector<const char*> const& resImageNames, 
		std::vector<const char*> const& refImageNames, 
		int base, bool is2D)
{
  double falseSplit = -1.0, falseMerge = -1.0;
  if (is2D) { // Evaluate 2D
    double fsSum = 0.0, fmSum = 0.0;
    int n = resImageNames.size(); 
    if (refImageNames.size() != n) 
      perr("Error: proposed and reference image number disagree...");
    for (int i = 0; i < n; ++i) {
      LabelImage::Pointer resImage = 
	readImage<LabelImage>(resImageNames[i]);
      LabelImage::Pointer refImage = 
	readImage<LabelImage>(refImageNames[i]);
      fsSum += getConditionalEntropy<LabelImage>(refImage, resImage, 
						 base, true);
      fmSum += getConditionalEntropy<LabelImage>(resImage, refImage, 
						 base, true);
    }
    falseSplit = fsSum / (double)n;
    falseMerge = fmSum / (double)n;
  } 
  else {// Evaluate 3D 
    LabelImage3::Pointer resImage = resImageNames.size() == 1? 
      readImage<LabelImage3>(resImageNames[0]): 
      readImage<LabelImage3>(resImageNames);
    LabelImage3::Pointer refImage = refImageNames.size() == 1? 
      readImage<LabelImage3>(refImageNames[0]): 
      readImage<LabelImage3>(refImageNames);
    falseSplit = getConditionalEntropy<LabelImage3>(refImage, resImage, 
						    base, true);
    falseMerge = getConditionalEntropy<LabelImage3>(resImage, refImage, 
						    base, true);
  }
  std::cout << falseSplit << " " << falseMerge << " " 
	    << falseSplit + falseMerge << std::endl;
}



int main (int argc, char* argv[])
{
  if (argc < 6) {
    std::cerr << "Usage: " << argv[0]
	      << " isEvaluate2D (0 - 3D, otherwise - 2D)"
	      << " base ('2', 'e' or '10')"
	      << " mode" << std::endl;
    std::cerr << "Mode 0: "
	      << " imageNum" 
	      << " startImageNo"
	      << " imageNoInc"
	      << " proposedImageNameTemplate"
	      << " referenceImageNameTemplate"
	      << std::endl;
    std::cerr << "Mode 1: "
	      << " {-p proposedImageName}"
	      << " {-r referenceImageName}"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  bool isEval2D = (atoi(argv[argi++]) == 0? false: true);
  int base = -1;
  if (strcmp(argv[argi], "2") == 0) base = 2;
  else if (strcmp(argv[argi], "e") == 0) base = 1;
  else if (strcmp(argv[argi], "10") == 0) base = 0;
  else perr("Error: unsupported entropy base...");
  ++argi;
  int mode = atoi(argv[argi++]);
  std::vector<const char*> resImageNames, refImageNames;
  bool freeVars = false;
  if (mode == 0) {
    int n = atoi(argv[argi++]);
    int start = atoi(argv[argi++]);
    int inc = atoi(argv[argi++]);
    const char* resImageNameTemplate = argv[argi++];
    const char* refImageNameTemplate = argv[argi++];
    resImageNames.reserve(n);
    refImageNames.reserve(n);
    for (int i = 0; i < n; ++i) {
      char* file = new char[1000];
      sprintf(file, resImageNameTemplate, start + inc * i);
      resImageNames.push_back(file);
      file = new char[1000];
      sprintf(file, refImageNameTemplate, start + inc * i);
      refImageNames.push_back(file);
    }
    freeVars = true;
  }
  else if (mode == 1) {
    resImageNames.reserve((argc - 3) / 4);
    refImageNames.reserve((argc - 3) / 4);
    while (argi < argc) {
      if (strcmp(argv[argi], "-p") == 0) 
	resImageNames.push_back(argv[argi + 1]);
      else if (strcmp(argv[argi], "-r") == 0)
	refImageNames.push_back(argv[argi + 1]);
      else break;
      argi += 2;
    }
  }
  else perr("Error: incorrect mode selection...");
  operation(resImageNames, refImageNames, base, isEval2D);
  if (freeVars) {
    for (std::vector<const char*>::iterator it = resImageNames.begin(); 
	 it != resImageNames.end(); ++it) delete[] *it;
    for (std::vector<const char*>::iterator it = refImageNames.begin(); 
	 it != refImageNames.end(); ++it) delete[] *it;
  }
  return EXIT_SUCCESS;
}
