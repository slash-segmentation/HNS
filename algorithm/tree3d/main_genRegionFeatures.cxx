#include "tree2d/tree2d_io.h"
#include "rc.h"
#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

const int IM_NUM = 1;
const int IM_RAW = 0;

void operation (const char* featFileName, const char* labelImageName, 
		const char* treeFileName, 
		std::vector<const char*> valImageNames)
{
  LabelImage3::Pointer labelImage = readImage<LabelImage3>(labelImageName);
  std::vector<FloatImage3::Pointer> valImages;
  valImages.reserve(valImageNames.size());
  for (std::vector<const char*>::const_iterator nit = 
	 valImageNames.begin(); nit != valImageNames.end(); ++nit)
    valImages.push_back(readImage<FloatImage3>(*nit));
  LabelImage3::Pointer canvas = createImage<LabelImage3>(labelImage, 0);
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges, &assignDouble);
  int n = tree.size();
  PointMap3 rmap, cmap, bmap;
  PointLabelMap3 lmap;
  getPointMap(rmap, lmap, cmap, labelImage, CRCONN3, false, true);
  getBorderPoints(bmap, labelImage, false);
  for (PointMap3::iterator bit = bmap.begin(); bit != bmap.end(); ++bit) 
    merge(cmap.find(bit->first)->second, bit->second, false);
  std::vector<bool> bi(n, false); // Border indicators
  getNodeBorderIndicators(bi, tree, bmap);
  std::vector<Box3> boxes(n);
  std::list<flist> feats;
  for (int i = 0; i < n; ++i) {
    fTree::TreeNode const* pn = &(tree[i]);
    if (pn->child0 >= 0 && pn->child1 >= 0)
      merge(rmap, cmap, lmap, tree[pn->child0].label, 
	    tree[pn->child1].label, pn->label, true, false);
    Points3 const* pr = &(rmap.find(pn->label)->second);
    Points3 const* pc = &(cmap.find(pn->label)->second);
    if (pn->child0 >= 0 && pn->child1 >= 0) 
      boxes[i] = Box3::getUnion(boxes[pn->child0], boxes[pn->child1]);
    else boxes[i] = pr->get_box();
    feats.push_back(flist());
    getRegionShapeFeatures(feats.back(), *pr, *pc, boxes[i], 
			   canvas, bi[i]);
    getRegionIntensityFeatures(feats.back(), *pc, valImages[IM_RAW]);
    // For display
    std::cerr << featFileName << " (" << i + 1 << "/" << n << ")"
	      << std::endl;
    // ~ For display
  }
  write2(featFileName, feats, false, ' ', '\n');
}



int main (int argc, char* argv[])
{
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0] 
	      << " labelImageName"
	      << " treeFileName"
	      << " rawImageName"
	      << " featureFielName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  std::vector<const char*> valImageNames;
  valImageNames.reserve(1);
  valImageNames.push_back(argv[argi++]);
  const char* featFileName = argv[argi++];
  operation(featFileName, labelImageName, treeFileName, valImageNames);
  return EXIT_SUCCESS;
}
