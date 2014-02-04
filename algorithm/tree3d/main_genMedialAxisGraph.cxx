#include "tree2d/tree2d_io.h"
#include "tree3d_util.h"
#include "rc.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

void operation (const char* graphFileName, const char* initSegImageName, 
		const char* treeFileName, Label label)
{
  LabelImage3::Pointer canvas = readImage<LabelImage3>(initSegImageName);
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges);
  std::set<Label> initLabels;
  for (fTree::const_iterator tit = tree.begin(); tit != tree.end(); 
       ++tit) 
    if (tit->label == label) {
      std::list<int> leaves;
      tree.get_leaves(leaves, tit->self);
      for (std::list<int>::const_iterator lit = leaves.begin(); 
	   lit != leaves.end(); ++lit) 
	initLabels.insert(tree[*lit].label);
      break;
    }
  if (initLabels.empty()) perr("Error: desired label not found...");
  for (itk::ImageRegionIterator<LabelImage3> 
	 iit(canvas, canvas->GetLargestPossibleRegion()); 
       !iit.IsAtEnd(); ++iit) 
    if (initLabels.count(iit.Get())) iit.Set(1);
    else iit.Set(0);
  removeExclusiveGaps<LabelImage3>(canvas, BGVAL, CRCONN3);
  canvas = binaryThin(canvas, NULL);
  MaGraph mag(0);
  getMedialAxisGraph(mag, canvas, NULL, MEDIAL_AXIS_CONN);
  writeMedialAxisGraph(graphFileName, mag);
}



int main (int argc, char* argv[])
{
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
	      << " initSegImageName"
	      << " treeFileName"
	      << " label"
	      << " medialAxisGraphFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  Label label = atol(argv[argi++]);
  const char* graphFileName = argv[argi++];
  operation(graphFileName, initSegImageName, treeFileName, label);
  return EXIT_SUCCESS;
}
