#include "tree2d_io.h"
#include "tree2d_util.h"
#include "tree_visit.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* resImageName, const char* initSegImageName, 
		std::vector<const char*> treeFileNames, 
		std::vector<const char*> mergeProbFileNames, 
		std::vector<const char*> priorProbFileNames, 
		bool labelCC, bool writeU16, const char* potImageName, 
		const char* logFileName)
{
  LabelImage::Pointer initSegImage = 
    readImage<LabelImage>(initSegImageName);
  int treeNum = treeFileNames.size();
  std::vector<std::list<fMerge> > merges(treeNum);
  std::vector<std::list<double> > mprobs(treeNum);
  std::vector<fTree> trees(treeNum);
// #pragma omp parallel for
  for (int i = 0; i < treeNum; ++i) {
    readMerges(merges[i], treeFileNames[i], NULL);
    getTree(trees[i], merges[i]);
    read(mprobs[i], mergeProbFileNames[i], 1);
    // Avoid exactly 0 merge probability
    for (std::list<double>::iterator it = mprobs[i].begin(); 
	 it != mprobs[i].end(); ++it) 
      if (fabs(*it) < EPSILON) *it += 1e-4;    
    getPotentials(trees[i], mprobs[i], 0);
  }
  if (priorProbFileNames.size() == treeNum) {
    for (int i = 0; i < treeNum; ++i) {
      std::list<double> priorProbs;
      read(priorProbs, priorProbFileNames[i], 1);
      std::list<double>::const_iterator ppit = priorProbs.begin();
      for (fTree::iterator tit = trees[i].begin(); 
	   tit != trees[i].end(); ++tit) {
	tit->data *= (fabs(*ppit) < EPSILON? *ppit + 1e-4: *ppit); 
	// tit->data = *ppit;
	++ppit;
      }
    }
  }
  std::list<IntPair> picks;
  resolve(picks, trees, &compDouble, &assignDouble);
  std::set<int> pickedTrees;
  for (std::list<IntPair>::const_iterator pit = picks.begin(); 
       pit != picks.end(); ++pit) pickedTrees.insert(pit->first);
  std::map<int, PointMap> rmaps;
  for (std::set<int>::const_iterator it = pickedTrees.begin(); 
       it != pickedTrees.end(); ++it)
    getPointMap(rmaps[*it], initSegImage, merges[*it], false, CRCONN);
  LabelImage::Pointer resImage = 
    createImage<LabelImage>(initSegImage, BGVAL);
  for (std::list<IntPair>::const_iterator it = picks.begin(); 
       it != picks.end(); ++it) {
    int i = it->first, j = it->second;
    setvs<LabelImage>(resImage, rmaps[i][trees[i][j].label], 1);
  }
  if (writeU16) {
    UInt16Image::Pointer outImage = 
      (labelCC? 
       labelConnectedComponents<LabelImage, UInt16Image>(resImage, BGVAL): 
       castImage<LabelImage, UInt16Image>(resImage));
    writeImage<UInt16Image>(resImageName, outImage);
  }
  else {
    if (labelCC) 
      resImage = 
	labelConnectedComponents<LabelImage, LabelImage>(resImage, BGVAL);
    writeImage<LabelImage>(resImageName, resImage);
  }
  if (potImageName != NULL) {
    FloatImage::Pointer potImage = 
      createImage<LabelImage, FloatImage>(initSegImage, 0.0);
    for (std::list<IntPair>::const_iterator it = picks.begin(); 
	 it != picks.end(); ++it) {
      int i = it->first, j = it->second;
      setvs<FloatImage>(potImage, rmaps[i][trees[i][j].label], 
			trees[i][j].data);
    }
    writeImage<FloatImage>(potImageName, potImage);
  }
  if (logFileName != NULL) {
    std::ofstream fs(logFileName);
    if (fs.is_open()) {
      for (std::list<IntPair>::const_iterator pit = picks.begin(); 
	   pit != picks.end(); ++pit)
	fs << pit->first << " " << trees[pit->first][pit->second].label 
	   << " " << trees[pit->first][pit->second].data << "\n";
      fs.close();
    }
    else perr("Error: cannot create log file...");
  }
}



int main (int argc, char* argv[])
{
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0] 
	      << " initialSegmentationImageName" 
	      << " {-t treeFileName}"
	      << " {-m mergeProbabilityFileName}"
	      << " [{-p priorProbabilityFileName}]"
	      << " [labelOutputBinaryImageConnectedComponents = 1]"
	      << " [writeToUInt16Image = 0]"
	      << " [potentialImageName = NULL]"
	      << " [logFileName = NULL]"
	      << " finalSegmentationImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  std::vector<const char*> treeFileNames, mergeProbFileNames;
  std::vector<const char*> priorProbFileNames;
  treeFileNames.reserve((argc - 3) / 2);
  mergeProbFileNames.reserve((argc - 3) / 2);
  priorProbFileNames.reserve((argc - 3) / 2);
  while (argi < argc - 1) {
    if (strcmp(argv[argi], "-t") == 0) {
      treeFileNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else if (strcmp(argv[argi], "-m") == 0) {
      mergeProbFileNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else if (strcmp(argv[argi], "-p") == 0) {
      priorProbFileNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else break;
  }
  if (treeFileNames.size() != mergeProbFileNames.size()) 
    perr("Error: tree and merge probability file number disagree...");
  if (priorProbFileNames.size() != 0 && 
      priorProbFileNames.size() != treeFileNames.size())
    perr("Error: tree and prior probability file number disagree...");
  bool labelCC = (argi < argc - 1 && atoi(argv[argi++]) == 0? 
		  false: true);
  bool writeU16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? 
		   true: false);
  const char* potImageName = 
    (argi < argc - 1 && strcmp(argv[argi++], "NULL")? argv[argi - 1]: NULL);
  const char* logFileName = 
    (argi < argc - 1 && strcmp(argv[argi++], "NULL")? argv[argi - 1]: NULL);
  const char* resImageName = argv[argi++];
  operation(resImageName, initSegImageName, treeFileNames, 
	    mergeProbFileNames, priorProbFileNames, 
	    labelCC, writeU16, potImageName, logFileName);
  return EXIT_SUCCESS;
}
