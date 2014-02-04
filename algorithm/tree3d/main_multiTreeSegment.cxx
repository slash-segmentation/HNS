#include "tree2d/tree2d_io.h"
#include "tree2d/tree_visit.h"
#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

void operation (const char* resImageName, const char* initSegImageName, 
		std::vector<const char*> const& treeFileNames, 
		std::vector<const char*> const& mergeProbFileNames, 
		std::vector<const char*> const& priorProbFileNames, 
		bool labelCC, bool write16)
{
  int n = treeFileNames.size();
  std::vector<fTree> trees(n);
  bool includePrior = priorProbFileNames.size() == treeFileNames.size();
  for (int i = 0; i < n; ++i) {
    std::list<fMerge> merges;
    readMerges(merges, treeFileNames[i], NULL);
    getTree(trees[i], merges);
    std::list<double> mprobs;
    read(mprobs, mergeProbFileNames[i], 1);
    getPotentials(trees[i], mprobs);
    if (includePrior) {
      std::list<double> pprobs;
      read(pprobs, priorProbFileNames[i], 1);
      std::list<double>::const_iterator pit = pprobs.begin();
      for (fTree::iterator tit = trees[i].begin(); tit != trees[i].end(); 
	   ++tit) {
	tit->data += *pit;
	++pit;
      }
    }
  }
  std::list<IntPair> picks;
  resolve(picks, trees, &compDouble, &assignDouble);
  // // For debug
  // for (std::list<IntPair>::const_iterator pit = picks.begin(); 
  //      pit != picks.end(); ++pit) 
  //   std::cout << pit->first << " " << pit->second << " " 
  // 	      << trees[pit->first][pit->second].data << std::endl;
  // std::cout << std::endl;
  // // ~ For debug
  // Paint
  std::map<Label, Label> init2final;
  Label labelToAssign = 1;
  for (std::list<IntPair>::const_iterator pit = picks.begin(); 
       pit != picks.end(); ++pit) {
    std::list<int> leaves;
    trees[pit->first].get_leaves(leaves, pit->second);
    for (std::list<int>::const_iterator lit = leaves.begin(); 
	 lit != leaves.end(); ++lit) 
      init2final[trees[pit->first][*lit].label] = labelToAssign;
    ++labelToAssign;
  }
  LabelImage3::Pointer image = readImage<LabelImage3>(initSegImageName);
  // Keep previous initial/final label to avoid some lookups
  Label prevInit = 0, prevFinal = 0;
  for (itk::ImageRegionIterator<LabelImage3>
	 iit(image, image->GetLargestPossibleRegion()); !iit.IsAtEnd(); 
       ++iit) {
    Label init = iit.Get();
    if (init != prevInit) {
      prevInit = init;
      prevFinal = init2final.find(init)->second;
    }
    iit.Set(prevFinal);
  }
  // Remove exclusive gaps
  removeExclusiveGaps<LabelImage3>(image, BGVAL, CRCONN3);
  // Output 
  if (write16) {
    UInt16Image3::Pointer outImage = 
      (labelCC? 
       labelConnectedComponents<LabelImage3, UInt16Image3>(image, BGVAL): 
       castImage<LabelImage3, UInt16Image3>(image));
    writeImage<UInt16Image3>(resImageName, outImage);
  }
  else {
    if (labelCC) 
      image = 
	labelConnectedComponents<LabelImage3, LabelImage3>(image, BGVAL);
    writeImage<LabelImage3>(resImageName, image);
  }
}



int main (int argc, char* argv[])
{
  if (argc < 7) {
    std::cerr << "Usage: " << argv[0]
	      << " initSegImageName"
	      << " {-t treeFileName}"
	      << " {-m mergeProbabilityFileName}"
	      << " [{-p priorProbabilityFileName}]"
	      << " [labelOutputBinaryImageConnectedComponents = 1]"
	      << " [writeToUInt16Image = 0]"
	      << " finalSegImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  std::vector<const char*> treeFileNames, mprobFileNames, pprobFileNames;
  treeFileNames.reserve((argc - 3) / 4);
  mprobFileNames.reserve((argc - 3) / 4);
  pprobFileNames.reserve((argc - 3) / 4);
  while (argi < argc - 1) {
    if (strcmp(argv[argi], "-t") == 0) {
      treeFileNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else if (strcmp(argv[argi], "-m") == 0) {
      mprobFileNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else if (strcmp(argv[argi], "-p") == 0) {
      pprobFileNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else break;
  }
  if (treeFileNames.size() != mprobFileNames.size()) 
    perr("Error: tree and merge probability file number disagree...");
  if (pprobFileNames.size() != 0 && 
      pprobFileNames.size() != treeFileNames.size()) 
    perr("Error: tree and prior probability file number disagree...");
  bool labelCC = argi < argc - 1 && atoi(argv[argi++]) == 0? false: true;
  bool write16 = argi < argc - 1 && atoi(argv[argi++]) != 0? true: false;
  const char* resImageName = argv[argi++];
  operation(resImageName, initSegImageName, treeFileNames, mprobFileNames, 
	    pprobFileNames, labelCC, write16);
  return EXIT_SUCCESS;
}
