#include "util/metric/metric_util.h"
#include "tree2d/tree2d_io.h"
#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

void operation (const char* labelFileName, const char* initSegImageName, 
		const char* treeFileName, const char* truthImageName, 
		double overlapRatioThreshold)
{
  LabelImage3::Pointer initSegImage = 
    readImage<LabelImage3>(initSegImageName);
  LabelImage3::Pointer truthImage = readImage<LabelImage3>(truthImageName);
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges, &assignDouble);
  Points3 bg;
  PointMap3 cmap;
  PointLabelMap3 lmap;
  getPoints(bg, initSegImage, BGVAL);
  getPointNeighbors(lmap, bg, initSegImage, CRCONN3);
  getNeighborPoints(cmap, lmap, true);
  std::map<Label, unsigned int> segSizeMap, truthSizeMap;
  getSizeMap<LabelImage3>(segSizeMap, initSegImage, false);
  getSizeMap<LabelImage3>(truthSizeMap, truthImage, false);
  // Compute seg-to-truth overlaps
  std::map<Label, std::map<Label, unsigned int> > stOverlaps;  
  getOverlaps(stOverlaps, initSegImage, truthImage, false, false);
  for (fTree::const_iterator tit = tree.begin(); tit != tree.end(); 
       ++tit)
    if (tit->child0 >= 0 && tit->child1 >= 0) {
      Label l0 = tree[tit->child0].label, l1 = tree[tit->child1].label;
      add(stOverlaps[tit->label], stOverlaps.find(l0)->second, 
	  stOverlaps.find(l1)->second);
      Points3 c2rp;
      merge(&c2rp, cmap, lmap, l0, l1, tit->label, true, false);
      segSizeMap[tit->label] = segSizeMap.find(l0)->second + 
	segSizeMap.find(l1)->second + c2rp.size();
    }
  // Compute truth-to-seg overlap ratios using seg-to-truth overlaps
  // (O / (A0 + A1 - O))
  std::map<Label, std::map<Label, double> > tsOverlapRatios;
  for (std::map<Label, std::map<Label, unsigned int> >::const_iterator 
	 stit = stOverlaps.begin(); stit != stOverlaps.end(); ++stit) {
    Label s = stit->first;
    unsigned int segSize = segSizeMap.find(s)->second;
    for (std::map<Label, unsigned int>::const_iterator oit = 
	   stit->second.begin(); oit != stit->second.end(); ++oit) {
      Label t = oit->first;
      tsOverlapRatios[t][s] = getr(oit->second, segSize + 
				   truthSizeMap.find(t)->second - 
				   oit->second);
    }
  }
  // For a truth region, choose a seg with max overlap ratio (>= threshold)
  std::map<Label, Label> matched; // (seg, truth)
  for (std::map<Label, std::map<Label, double> >::const_iterator 
	 tsit = tsOverlapRatios.begin(); tsit != tsOverlapRatios.end(); 
       ++tsit) {
    std::pair<Label, double> mr;
    if (findMax(mr, tsit->second) && mr.second >= overlapRatioThreshold)
      matched[mr.first] = tsit->first;
  }
  std::list<int> labels;
  for (fTree::const_iterator tit = tree.begin(); tit != tree.end(); 
       ++tit) labels.push_back(matched.count(tit->label) == 0? -1: 1);
  write(labelFileName, labels, 1);
}


int main (int argc, char* argv[])
{
  if (argc != 5 && argc != 6) {
    std::cerr << "Usage: " << argv[0]
	      << " initSegImageName"
	      << " treeFileName"
	      << " truthImageName"
	      << " [overlapRatioThreshold = 0.85]"
	      << " labelFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  const char* truthImageName = argv[argi++];
  double overlapRatioThreshold = argi < argc - 1? atof(argv[argi++]): 0.85;
  const char* labelFileName = argv[argi++];
  operation(labelFileName, initSegImageName, treeFileName, truthImageName, 
	    overlapRatioThreshold);
  return EXIT_SUCCESS;
}
