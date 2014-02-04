#include "tree2d/tree2d_io.h"
#include "tree2d/tree_visit.h"
#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

void operation (const char* resImageName, const char* initSegImageName, 
		const char* treeFileName, const char* mergeProbFileName, 
		const char* priorProbFileName, bool labelCC, 
		bool write16)
{
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges);
  std::list<double> mprobs, pprobs;
  read(mprobs, mergeProbFileName, 1);
  read(pprobs, priorProbFileName, 1);
  getPotentials(tree, mprobs);
  std::list<double>::const_iterator pit = pprobs.begin();
  for (fTree::iterator tit = tree.begin(); tit != tree.end(); ++tit) {
    // tit->data *= (*pit + 1e-3);
    tit->data += *pit;
    ++pit;
  }
  std::list<int> picks;
  resolve(picks, tree);
  std::map<Label, Label> init2final;
  Label labelToAssign = 1;
  for (std::list<int>::const_iterator pit = picks.begin(); 
       pit != picks.end(); ++pit) {
    std::list<int> leaves;
    tree.get_leaves(leaves, *pit);
    for (std::list<int>::const_iterator lit = leaves.begin(); 
	 lit != leaves.end(); ++lit) 
      init2final[tree[*lit].label] = labelToAssign;
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
  if (argc < 6 || argc > 8) {
    std::cerr << "Usage: " << argv[0]
	      << " initialSegmentationImageName"
	      << " treeFileName"
	      << " mergeProbabilityFileName"
	      << " priorProbabilityFileName"
	      << " [labelOutputBinaryImageConnectedComponents = 1]"
	      << " [writeToUInt16Image = 0]"
	      << " finalSegmentationImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  const char* mergeProbFileName = argv[argi++];
  const char* priorProbFileName = argv[argi++];
  bool labelCC = (argi < argc - 1 && atoi(argv[argi++]) == 0? false: true);
  bool write16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false);
  const char* resImageName = argv[argi++];
  operation(resImageName, initSegImageName, treeFileName, 
	    mergeProbFileName, priorProbFileName, labelCC, write16);
  return EXIT_SUCCESS;
}
