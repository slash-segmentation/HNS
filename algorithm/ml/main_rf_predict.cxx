#include "util/ml/ml_rf.h"
using namespace rf;

void writePredictionToFile (const char* predFileName, double* votes, 
			    int N, int nclass, int classNo, int ntree)
{
  std::ofstream fs(predFileName);
  if (fs.is_open()) {
    for (int i = 0; i < N; i++) 
      fs << votes[i * nclass + classNo] / (double)ntree << std::endl;
    fs.close();
  }
  else {
    std::cerr << "Error creating prediction file..." << std::endl;
    exit(EXIT_FAILURE);
  }
}



void operation (std::vector<const char*>& predFileNames, 
		const char* modelFileName, int label, 
		std::vector<const char*> const& featFileNames)
{
  Model model;
  readModelFromBinaryFile(model, modelFileName);
  int n_orig_labels = model.n_orig_labels[0] * model.n_orig_labels[1];
  int classNo = -1;
  for (int i = 0; i < n_orig_labels; i++) {
    if (model.orig_labels[i] == label) {
      classNo = i;
      break;
    }
  }
  if (classNo == -1) {
    std::cerr << "Invalid label..." << std::endl;
    exit(EXIT_FAILURE);
  }
  double* X = NULL;
  int N, D;
  int* Y = NULL;
  double* votes = NULL;
  int* prediction_per_tree = NULL;
  double* proximity_ts = NULL;
  int* nodes = NULL;
  PredictExtraOptions options;
  int fileNum = featFileNames.size();  
  for (int i = 0; i < fileNum; i++) {
    readMatrixFromFile(X, N, D, featFileNames[i]);
    predict(Y, votes, prediction_per_tree, proximity_ts, nodes, 
	    X, N, D, model, options);
    writePredictionToFile(predFileNames[i], votes, N, model.nclass, 
			  classNo, model.ntree);
    del(&X);
    del(&Y);
    del(&votes);
    del(&prediction_per_tree);
    del(&proximity_ts);
    del(&nodes);
    // std::cerr << predFileNames[i] << std::endl;
  }
}



int main (int argc, char* argv[])
{
  if (argc < 7) {
    std::cerr << "Usage: " << argv[0]
	      << " modelFileName" 
	      << " label" 
	      << " {-f featureFileName}"
	      << " {-p predictionFileName}"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argNo = 1;
  const char* modelFileName = argv[argNo++];
  int label = atoi(argv[argNo++]);
  std::vector<const char*> featFileNames, predFileNames;
  featFileNames.reserve((argc - 3) / 4);
  predFileNames.reserve((argc - 3) / 4);
  while (argNo < argc) {
    if (strcmp(argv[argNo], "-f") == 0) {
      featFileNames.push_back(argv[argNo + 1]);
      argNo += 2;
    }
    else if (strcmp(argv[argNo], "-p") == 0) {
      predFileNames.push_back(argv[argNo + 1]);
      argNo += 2;
    }
    else {
      std::cerr << "Unsupported argument tag..." << std::endl;
      return EXIT_FAILURE;
    }
  }
  if (featFileNames.size() != predFileNames.size()) {
    std::cerr << "Feature file and prediction file numbers mismatch..." 
	      << std::endl;
    return EXIT_FAILURE;
  }
  operation(predFileNames, modelFileName, label, featFileNames);
  return EXIT_SUCCESS;
}
