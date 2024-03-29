#include "tree2d/tree2d_io.h"
#include "rc.h"
#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

const int IM_NUM = 1;
const int IM_RAW = 0;

void operation (const char* featFileName, const char* initSegImageName, 
		const char* treeFileName, 
		std::vector<const char*> const& valImageNames, 
		double limbThreshold)
{
  LabelImage3::Pointer initSegImage = 
    readImage<LabelImage3>(initSegImageName);
  std::vector<FloatImage3::Pointer> valImages;
  valImages.reserve(valImageNames.size());
  for (std::vector<const char*>::const_iterator nit = 
	 valImageNames.begin(); nit != valImageNames.end(); ++nit)
    valImages.push_back(readImage<FloatImage3>(*nit));
  LabelImage3::Pointer canvas = createImage<LabelImage3>(initSegImage, 0);
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges, &assignDouble);
  int n = tree.size();
  PointMap3 rmap, cmap, bmap;
  PointLabelMap3 lmap;
  getPointMap(rmap, lmap, cmap, initSegImage, CRCONN3, false, true);
  getBorderPoints(bmap, initSegImage, false);
  // std::vector<Box3> boxes(n);
  std::list<flist> feats;
  for (int i = 0; i < n; ++i) {
    fTree::TreeNode const* pn = &(tree[i]);
    if (pn->child0 >= 0 && pn->child1 >= 0)
      merge(rmap, cmap, lmap, bmap, tree[pn->child0].label, 
	    tree[pn->child1].label, pn->label, true, false);
    Points3 const* pr = &(rmap.find(pn->label)->second);
    Points3 const* pc = &(cmap.find(pn->label)->second);
    PointMap3::const_iterator bit = bmap.find(pn->label);
    // if (pn->child0 >= 0 && pn->child1 >= 0) 
    //   boxes[i] = Box3::getUnion(boxes[pn->child0], boxes[pn->child1]);
    // else boxes[i] = pr->get_box();
    feats.push_back(flist());
    // getRegionShapeFeatures(feats.back(), *pr, *pc, boxes[i], 
    // 			   canvas, limbThreshold, bi[i]);
    getRegionShapeFeatures(feats.back(), *pr, *pc, 
			   bit == bmap.end()? NULL: &(bit->second), 
			   NULL, canvas, limbThreshold);
    getRegionIntensityFeatures(feats.back(), *pc, valImages[IM_RAW]);
    // // For display
    // std::cout << featFileName << " (" << i + 1 << "/" << n << ")"
    // 	      << std::endl;
    // // ~ For display
  }
  write2(featFileName, feats, false, ' ', '\n');
}



int main (int argc, char* argv[])
{
  if (argc < 5 || argc > 6) {
    std::cerr << "Usage: " << argv[0] 
	      << " initSegImageName"
	      << " treeFileName"
	      << " rawImageName"
	      << " [medialAxisLimbThreshold = 50]"
	      << " featureFielName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  std::vector<const char*> valImageNames;
  valImageNames.reserve(1);
  valImageNames.push_back(argv[argi++]);
  double limbThreshold = argi < argc - 1? atof(argv[argi++]): 50.0;
  const char* featFileName = argv[argi++];
  operation(featFileName, initSegImageName, treeFileName, valImageNames, 
	    limbThreshold);
  return EXIT_SUCCESS;
}
