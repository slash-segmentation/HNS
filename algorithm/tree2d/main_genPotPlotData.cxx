#include "util/metric/metric_util.h"
#include "tree2d_util.h"
#include "tree2d_io.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* dataFileName, const char* initSegImageName, 
		const char* treeFileName, const char* mergeProbFileName, 
		const char* truthImageName)
{
  LabelImage::Pointer initSegImage = 
    readImage<LabelImage>(initSegImageName);
  LabelImage::Pointer truthImage = readImage<LabelImage>(truthImageName);
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges);
  PointMap rmap;
  getPointMap(rmap, initSegImage, merges, false, CRCONN, true);
  std::list<double> mprobs;
  read(mprobs, mergeProbFileName, 1);
  getPotentials(tree, mprobs);
  std::map<Label, unsigned int> truthSizeMap;
  getSizeMap<LabelImage>(truthSizeMap, truthImage, false);
  double npix = getw<LabelImage>(truthImage) * geth<LabelImage>(truthImage);
  std::list<double> outputs; // potential + max covering  
  for (fTree::const_iterator tit = tree.begin(); 
       tit != tree.end(); ++tit) {
    outputs.push_back(tit->data);
    Points const* ps = &(rmap.find(tit->label)->second);
    std::pair<Label, double> m;
    outputs.push_back(match(m, *ps, ps->size(), truthImage, 
    			    truthSizeMap)? m.second: 0.0);
  }
  write(dataFileName, outputs, 2, ' ');
}



int main (int argc, char* argv[])
{
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0] 
	      << " initSegImageName"
	      << " treeFileName"
	      << " mergeProbFileName"
	      << " truthImageName"
	      << " dataFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  const char* mergeProbFileName = argv[argi++];
  const char* truthImageName = argv[argi++];
  const char* dataFileName = argv[argi++];
  operation(dataFileName, initSegImageName, treeFileName, 
	    mergeProbFileName, truthImageName);
  return EXIT_SUCCESS;
}
