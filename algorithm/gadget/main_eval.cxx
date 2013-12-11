#include "util/metric/metric_pairf.h"
using namespace n3;

// void operation (std::vector<const char*> const& resImageNames, 
// 		std::vector<const char*> const& refImageNames, 
// 		bool isEval2D, bool doThreshold, int thresholdNum, 
// 		double lowerBound, double upperBound, bool invert)
// {
//   int n = resImageNames.size();
//   if (n != refImageNames.size()) perr("Error: image numbers disagree...");
//   std::vector<LabelImage::Pointer> refImages(n);
// #pragma omp parallel for
//     for (int i = 0; i < n; ++i)
//       refImages[i] = readImage<LabelImage>(refImageNames[i]);    
//   if (doThreshold) {
//     std::vector<FloatImage::Pointer> resImages(n);
// #pragma omp parallel for
//     for (int i = 0; i < n; ++i) {
//       resImages[i] = readImage<FloatImage>(resImageNames[i]);
//       if (invert) 
// 	resImages[i] = invertImage<FloatImage>(resImages[i], upperBound);
//     }
//     double thresholdInc = (upperBound - lowerBound) / (thresholdNum - 1);
//     double threshold = lowerBound;
//     double bestThreshold = lowerBound;
//     BigFloat bestPrecision = -1.0, bestRecall = -1.0, bestF = -1.0;
//     for (int i = 0; i < thresholdNum; ++i) {
//       std::vector<LabelImage::Pointer> thresholded(n);
// #pragma omp parallel for 
//       for (int j = 0; j < n; ++j) {
// 	thresholded[j] = 
// 	  thresholdImage<FloatImage, LabelImage>(resImages[j], lowerBound, 
// 						 threshold, 1, 0);
// 	thresholded[j] = 
// 	  labelConnectedComponents<LabelImage, LabelImage>(thresholded[j], 
// 							   0, false);
//       }
//       BigInt tp = 0, tn = 0, fp = 0, fn = 0;
//       std::vector<BigInt> tps(n), tns(n), fps(n), fns(n);
// #pragma omp parallel for 
//       for (int j = 0; j < n; ++j) 
// 	getPairScores(tps[j], tns[j], fps[j], fns[j], thresholded[j], 
// 		      refImages[j]);
//       for (int j = 0; j < n; ++j) {
// 	tp += tps[j];
// 	fp += fps[j];
// 	fn += fns[j];
//       }
//       BigFloat precision = getPrecision(tp, fp);
//       BigFloat recall = getRecall(tp, fn);
//       BigFloat f = getF(precision, recall);
//       if (f > bestF) {
// 	bestPrecision = precision;
// 	bestRecall = recall;
// 	bestF = f;
// 	bestThreshold = threshold;
//       }
//       std::cout << precision << " " << recall << " " << 1.0 - f << " [" 
// 		<< threshold << "]" << std::endl;
//       threshold += thresholdInc;
//     }
//     std::cout << "Best: " << bestPrecision << " " << bestRecall << " "
// 	      << 1.0 - bestF << " [" << bestThreshold << "]" << std::endl;
//   }
//   else {
//     std::vector<LabelImage::Pointer> resImages(n);
// #pragma omp parallel for
//     for (int i = 0; i < n; ++i)
//       resImages[i] = readImage<LabelImage>(resImageNames[i]);
//     BigInt tp = 0, tn = 0, fp = 0, fn = 0;
//     // Evaluate 2D
//     if (isEval2D) {
//       std::vector<BigInt> tps(n), tns(n), fps(n), fns(n);
// #pragma omp parallel for
//       for (int i = 0; i < n; ++i)
// 	getPairScores(tps[i], tns[i], fps[i], fns[i], resImages[i], 
// 		      refImages[i]);
//       for (int i = 0; i < n; ++i) {
// 	tp += tps[i];
// 	fp += fps[i];
// 	fn += fns[i];
//       }
//     }
//     // Evaluate 3D
//     else getPairScores(tp, tn, fp, fn, resImages, refImages);
//     BigFloat precision = getPrecision(tp, fp);
//     BigFloat recall = getRecall(tp, fn);
//     BigFloat f = getF(precision, recall);
//     std::cout << precision << " " << recall << " " << 1.0 - f << std::endl;
//   }
// }



void operation (std::vector<const char*> const& resImageNames, 
		std::vector<const char*> const& refImageNames, 
		bool isEval2D, bool doThreshold, int thresholdNum, 
		double lowerBound, double upperBound, bool invert)
{
  int n = resImageNames.size();
  if (n != refImageNames.size()) perr("Error: image numbers disagree...");
  if (isEval2D) { // Evaluate 2D
    if (doThreshold) {
      std::vector<LabelImage::Pointer> refImages(n);
      std::vector<FloatImage::Pointer> resImages(n);
#pragma omp parallel for 
      for (int i = 0; i < n; ++i) {
	refImages[i] = readImage<LabelImage>(refImageNames[i]);
	resImages[i] = readImage<FloatImage>(resImageNames[i]);
      }      
      double thresholdInc = (upperBound - lowerBound) / (thresholdNum - 1);
      double threshold = lowerBound;
      double bestThreshold = lowerBound;
      BigFloat bestPrec = -1.0, bestRec = -1.0, bestF = -1.0;
      for (int i = 0; i < thresholdNum; ++i) {
	std::vector<LabelImage::Pointer> thresholded(n);
#pragma omp parallel for 
	for (int j = 0; j < n; ++j)
	  thresholded[j] = labelConnectedComponents<LabelImage, LabelImage>
	    (thresholdImage<FloatImage, LabelImage>
	     (resImages[j], lowerBound, threshold, 1, 0), 0, false);
	std::vector<BigInt> tps(n), tns(n), fps(n), fns(n);
#pragma omp parallel for 
	for (int j = 0; j < n; ++j)
	  getPairScores(tps[j], tns[j], fps[j], fns[j], thresholded[j], 
			refImages[j]);
	BigInt tp = 0, fp = 0, fn = 0;
	for (int j = 0; j < n; ++j) {
	  tp += tps[j];
	  fp += fps[j];
	  fn += fns[j];
	}
	BigFloat prec = getPrecision(tp, fp), rec = getRecall(tp, fn);
	BigFloat f = getF(prec, rec);
	if (f > bestF) {
	  bestPrec = prec;
	  bestRec = rec;
	  bestF = f;
	  bestThreshold = threshold;
	}
	std::cout << prec << " " << rec << " " << 1.0 - f << " [" 
		  << threshold << "]" << std::endl;
	threshold += thresholdInc;
      }
      std::cout << "Best: " << bestPrec << " " << bestRec << " "
		<< 1.0 - bestF << " [" << bestThreshold << "]" 
		<< std::endl;
    }
    else {
      std::vector<BigInt> tps(n), tns(n), fps(n), fns(n);
#pragma omp parallel for
      for (int i = 0; i < n; ++i) {
	LabelImage::Pointer resImage = 
	  readImage<LabelImage>(resImageNames[i]);
	LabelImage::Pointer refImage = 
	  readImage<LabelImage>(refImageNames[i]);
	getPairScores(tps[i], tns[i], fps[i], fns[i], resImage, refImage);
      }
      BigInt tp = 0, fp = 0, fn = 0;
      for (int i = 0; i < n; ++i) {
	tp += tps[i];
	fp += fps[i];
	fn += fns[i];
      }
      BigFloat prec = getPrecision(tp, fp), rec = getRecall(tp, fn);
      BigFloat f = getF(prec, rec);
      std::cout << prec << " " << rec << " " << 1.0 - f << std::endl;
    }
  }
  else { // Evaluate 3D
    LabelImage3::Pointer refImage;
    refImage = (n == 1? readImage<LabelImage3>(refImageNames[0]): 
		readImage<LabelImage3>(refImageNames));
    if (doThreshold) {
      FloatImage3::Pointer resImage = 
	(n == 1? readImage<FloatImage3>(resImageNames[0]): 
	 readImage<FloatImage3>(resImageNames));
      double thresholdInc = (upperBound - lowerBound) / (thresholdNum - 1);
      double threshold = lowerBound;
      double bestThreshold = lowerBound;
      BigFloat bestPrec = -1.0, bestRec = -1.0, bestF = -1.0;
      for (int i = 0; i < thresholdNum; ++i) {
	LabelImage3::Pointer thresholded = 
	  labelConnectedComponents<LabelImage3, LabelImage3>
	  (thresholdImage<FloatImage3, LabelImage3>
	   (resImage, lowerBound, threshold, 1, 0), 0, false);
	BigInt tp = 0, tn = 0, fp = 0, fn = 0;
	getPairScores(tp, tn, fp, fn, thresholded, refImage);
	BigFloat prec = getPrecision(tp, fp), rec = getRecall(tp, fn);
	BigFloat f = getF(prec, rec);
	if (f > bestF) {
	  bestPrec = prec;
	  bestRec = rec;
	  bestF = f;
	  bestThreshold = threshold;
	}
	std::cout << prec << " " << rec << " " << 1.0 - f << " [" 
		  << threshold << "]" << std::endl;
	threshold += thresholdInc;
      }
      std::cout << "Best: " << bestPrec << " " << bestRec << " "
		<< 1.0 - bestF << " [" << bestThreshold << "]" 
		<< std::endl;
    }
    else {
      LabelImage3::Pointer resImage = 
	(n == 1? readImage<LabelImage3>(resImageNames[0]): 
	 readImage<LabelImage3>(resImageNames));
      BigInt tp = 0, tn = 0, fp = 0, fn = 0;
      getPairScores(tp, tn, fp, fn, resImage, refImage);
      BigFloat prec = getPrecision(tp, fp), rec = getRecall(tp, fn);
      BigFloat f = getF(prec, rec);
      std::cout << prec << " " << rec << " " << 1.0 - f << std::endl;      
    }
  }
}



int main (int argc, char* argv[])
{
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0]
	      << " isEvaluate2D (0 - 3D, otherwise - 2D)"
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
    std::cerr << "[doThreshold = 0]"
	      << " [thresholdNum = 11]"
	      << " [lowerBound = 0.0]"
	      << " [upperBound = 1.0]"
	      << " [invert = 0 (default: foreground < background)]"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  bool isEval2D = (atoi(argv[argi++]) == 0? false: true);
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
  bool doThreshold = 
    argi < argc && atoi(argv[argi++]) != 0? true: false;
  int thresholdNum = argi < argc? atoi(argv[argi++]): 11;
  double lowerBound = argi < argc? atof(argv[argi++]): 0.0;
  double upperBound = argi < argc? atof(argv[argi++]): 1.0;
  bool invert = argi < argc && atoi(argv[argi++]) != 0? true: false;
  operation(resImageNames, refImageNames, isEval2D, doThreshold, 
	    thresholdNum, lowerBound, upperBound, invert);
  if (freeVars) {
    for (std::vector<const char*>::iterator it = resImageNames.begin(); 
	 it != resImageNames.end(); ++it) delete[] *it;
    for (std::vector<const char*>::iterator it = refImageNames.begin(); 
	 it != refImageNames.end(); ++it) delete[] *it;
  }
  return EXIT_SUCCESS;
}
