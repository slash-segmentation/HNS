#include "util/ml/ml_rf.h"
using namespace rf;

void operation (const char* modelFileName, 
		std::vector<const char*> featFileNames, 
		std::vector<const char*> labelFileNames, 
		int ntree, int mtry, float sampsizeRatio, 
		bool isBalanceClass)
{
  double* X;
  int N, D;
  readMatrixFromFiles(X, N, D, featFileNames);
  int* Y;
  int Ny, Dy;
  readMatrixFromFiles(Y, Ny, Dy, labelFileNames);
  if (N != Ny || Dy != 1) {
    std::cerr << "Error matrices dimension..." << std::endl;
    exit(EXIT_FAILURE);
  }
  TrainExtraOptions options;
  // Set sample size
  if (sampsizeRatio > 1e-6) {
    options.sampsize = 
      createIntScalar((int)((float)N * sampsizeRatio + 0.5));
    options.n_sampsize = 1;
  }
  // Set classwt to balance class
  if (isBalanceClass) {
    std::map<int, int> labelCount;
    countLabel(labelCount, Y, N);
    int maxcnt = -1;
    for (std::map<int, int>::const_iterator citr = labelCount.begin(); 
	 citr != labelCount.end(); citr++) {
      if (citr->second > maxcnt) maxcnt = citr->second;
    }
    options.classwt = new double[labelCount.size()];
    options.n_classwt = labelCount.size();
    int i = 0;
    for (std::map<int, int>::const_iterator citr = labelCount.begin(); 
	 citr != labelCount.end(); citr++) {
      options.classwt[i] = (double)maxcnt / (double)citr->second;
      std::cerr << "Class " << citr->first << ": " << citr->second 
		<< " [w = " << options.classwt[i] << "]" << std::endl;
      i++;
    }
  }
  Model model;
  train(model, X, Y, N, D, options, ntree, mtry);
  writeModelToBinaryFile(modelFileName, model);
  delete[] X;
  delete[] Y;
}



int main (int argc, char* argv[])
{
  if (argc < 10) {
    std::cerr << "Usage: " << argv[0]
	      << " {-f featureFileName}" 
	      << " {-l labelFileName}" 
	      << " treeNum (0 for default: 500)"
	      << " mtry (0 for default: sqrt(D))" 
	      << " sampleRate (0 for default: 1)"
	      << " isBalanceClass"
	      << " modelFileName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argNo = 1;
  std::vector<const char*> featFileNames, labelFileNames;
  featFileNames.reserve((argc - 6) / 4);
  labelFileNames.reserve((argc - 6) / 4);
  while (argNo < argc - 5) {
    if (strcmp(argv[argNo], "-f") == 0) {
      featFileNames.push_back(argv[argNo + 1]);
      argNo += 2;
    }
    else if (strcmp(argv[argNo], "-l") == 0) {
      labelFileNames.push_back(argv[argNo + 1]);
      argNo += 2;
    }
    else {
      std::cerr << "Unsupported argument tag..." << std::endl;
      return EXIT_FAILURE;
    }
  }
  if (featFileNames.size() != labelFileNames.size()) {
    std::cerr << "Feature file and label file numbers mismatch..." 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int ntree = atoi(argv[argNo++]);
  int mtry = atoi(argv[argNo++]);
  float sampsizeRatio = atof(argv[argNo++]);
  bool isBalanceClass = atoi(argv[argNo++]) == 0? false: true;
  const char* modelFileName = argv[argNo++];
  operation(modelFileName, featFileNames, labelFileNames, ntree, mtry, 
	    sampsizeRatio, isBalanceClass);
  return EXIT_SUCCESS;
}
