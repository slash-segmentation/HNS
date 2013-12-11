#include "tree2d_io.h"
#include "tree2d_util.h"
#include "tree_visit.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* resImageName, const char* initSegImageName, 
		const char* treeFileName, const char* mergeProbFileName, 
		const char* priorProbFileName, bool labelCC, 
		bool writeU16, const char* potImageName)
{
  LabelImage::Pointer initSegImage = 
    readImage<LabelImage>(initSegImageName);
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges);
  std::list<double> mprobs;
  read(mprobs, mergeProbFileName, 1);
  getPotentials(tree, mprobs);
  if (priorProbFileName != NULL) {
    std::list<double> priorProbs;
    read(priorProbs, priorProbFileName, 1);
    std::list<double>::const_iterator ppit = priorProbs.begin();
    for (fTree::iterator tit = tree.begin(); tit != tree.end(); ++tit) {
      double prior = *ppit;
      if (fabs(prior) < 1e-4) prior += 1e-4;
      tit->data * prior;
      ++ppit;
    }
  } 
  PointMap rmap;
  getPointMap(rmap, initSegImage, merges, false, CRCONN);
  std::list<int> picks;
  int i = getPick(tree);
  int order = 1;
  while (i >= 0) {    
    std::list<int> indices;
    tree.get_descendants(indices, i);
    // If no descendants, pick i
    if (indices.size() == 0) indices.push_back(i);
    int j = getPick(tree, indices);
    while (j >= 0) {
      picks.push_back(j);
      std::list<BiTreeNodeChange<double> > changes;
      BiTreeNodeChange<double> c(tree[j]);
      c.setPost("status", order++);
      changes.push_back(c);
      removeAncestors(changes, tree, j, -1);
      removeDescendants(changes, tree, j, -1);
      tree.do_changes(changes);
      j = getPick(tree, indices);
    }
    i = getPick(tree);
  }
  LabelImage::Pointer resImage = 
    createImage<LabelImage>(initSegImage, BGVAL);
  for (std::list<int>::const_iterator it = picks.begin(); 
       it != picks.end(); ++it)
    setvs<LabelImage>(resImage, rmap[tree[*it].label], 1);
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
    for (std::list<int>::const_iterator it = picks.begin(); 
	 it != picks.end(); ++it) 
      setvs<FloatImage>(potImage, rmap[tree[*it].label], tree[*it].data);
    writeImage<FloatImage>(potImageName, potImage);
  }
}



int main (int argc, char* argv[])
{
  if (argc < 6 || argc > 9) {
    std::cerr << "Usage: " << argv[0]
	      << " initialSegmentationImageName" 
	      << " treeFileName" 
	      << " mergeProbabilityFileName" 
	      << " priorProbabilityFileName"
	      << " [labelOutputBinaryImageConnectedComponents = 1]"
	      << " [writeToUInt16Image = 0]"
	      << " [potentialImageName = NULL]"
	      << " finalSegmentationImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  const char* mergeProbFileName = argv[argi++];
  const char* priorProbFileName = argv[argi++];
  bool labelCC = (argi < argc - 1 && atoi(argv[argi++]) == 0? 
		  false: true);
  bool writeU16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? 
		   true: false);
  const char* potImageName = 
    (argi < argc - 1 && strcmp(argv[argi++], "NULL")? argv[argi - 1]: NULL);
  const char* resImageName = argv[argi++];
  operation(resImageName, initSegImageName, treeFileName, 
	    mergeProbFileName, priorProbFileName, labelCC, writeU16, 
	    potImageName);
  return EXIT_SUCCESS;
}
