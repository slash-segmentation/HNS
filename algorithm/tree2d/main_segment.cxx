#include "tree2d_io.h"
#include "tree_visit.h"
#include "tree2d_util.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* resImageName, const char* initSegImageName, 
		const char* treeFileName, const char* mergeProbFileName, 
		bool labelCC, bool write16)
{
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges);
  std::list<double> mprobs;
  read(mprobs, mergeProbFileName, 1);
  getPotentials(tree, mprobs);
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
  LabelImage::Pointer image = readImage<LabelImage>(initSegImageName);
  // Keep previous initial/final label to avoid some lookups
  Label prevInit = 0, prevFinal = 0;
  for (itk::ImageRegionIterator<LabelImage>
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
  removeExclusiveGaps<LabelImage>(image, BGVAL, CRCONN);
  // Output 
  if (write16) {
    UInt16Image::Pointer outImage = 
      (labelCC? 
       labelConnectedComponents<LabelImage, UInt16Image>(image, BGVAL): 
       castImage<LabelImage, UInt16Image>(image));
    writeImage<UInt16Image>(resImageName, outImage);
  }
  else {
    if (labelCC) 
      image = 
	labelConnectedComponents<LabelImage, LabelImage>(image, BGVAL);
    writeImage<LabelImage>(resImageName, image);
  }
}



int main (int argc, char* argv[])
{
  if (argc < 4 || argc > 7) {
    std::cerr << "Usage: " << argv[0]
	      << " initialSegmentationImageName" 
	      << " treeFileName" 
	      << " mergeProbabilityFileName" 
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
  bool labelCC = (argi < argc - 1 && atoi(argv[argi++]) == 0? false: true);
  bool write16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false);
  const char* resImageName = argv[argi++];
  operation(resImageName, initSegImageName, treeFileName, 
	    mergeProbFileName, labelCC, write16);
  return EXIT_SUCCESS;
}
