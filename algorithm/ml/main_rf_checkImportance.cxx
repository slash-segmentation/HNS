#include "util/ml/ml_rf.h"
using namespace rf;

void operation (const char* modelFileName, std::vector<int> const& featNos)
{
  Model model;
  readModelFromBinaryFile(model, modelFileName);
  double sum = 0.0;
  if (featNos.size() == 0) {
    int featNum = model.n_importance[0] * model.n_importance[1];
    for (int featNo = 0; featNo < featNum; featNo++) {
      // std::cout << featNo << " " << model.importance[featNo] << std::endl;
      sum += model.importance[featNo];
    }
  }
  else {
    for (std::vector<int>::const_iterator nitr = featNos.begin(); 
	 nitr != featNos.end(); nitr++) {
      // std::cout << *nitr << " " << model.importance[*nitr] << std::endl;
      sum += model.importance[*nitr - 1];
    }
  }
  if (featNos.size() == 0) {
    int featNum = model.n_importance[0] * model.n_importance[1];
    for (int featNo = 0; featNo < featNum; featNo++) {
      std::cout << featNo + 1 << " " << model.importance[featNo] << " "
		<< model.importance[featNo] / sum << std::endl;
    }
  }
  else {
    for (std::vector<int>::const_iterator nitr = featNos.begin(); 
	 nitr != featNos.end(); nitr++) {
      std::cout << *nitr << " " << model.importance[*nitr - 1] << " "
		<< model.importance[*nitr - 1] / sum << std::endl;
    }
  }
}



int main (int argc, char* argv[]) 
{
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] 
	      << " modelFileName" 
	      << " [{featureNo (starting from 1)}]"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argNo = 1;
  const char* modelFileName = argv[argNo++];
  std::vector<int> featNos;
  featNos.reserve(argc - argNo);
  while (argNo < argc) featNos.push_back(atoi(argv[argNo++]));
  operation(modelFileName, featNos);
  return EXIT_SUCCESS;
}
