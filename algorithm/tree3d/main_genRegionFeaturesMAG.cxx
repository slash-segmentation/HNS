#include "tree2d/tree2d_io.h"
#include "rc.h"
#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

const int IM_NUM = 2;
const int IM_RAW = 0;
const int IM_PB = 1;

void operation (const char* featFileName, const char* initSegImageName, 
		const char* treeFileName, 
		std::vector<const char*> const& valImageNames, 
		std::map<Label, const char*> const& magFileNames, 
		double limbThreshold)
{
  LabelImage3::Pointer initSegImage = 
    readImage<LabelImage3>(initSegImageName);
  std::vector<FloatImage3::Pointer> valImages;
  valImages.reserve(valImageNames.size());
  for (std::vector<const char*>::const_iterator nit = 
	 valImageNames.begin(); nit != valImageNames.end(); ++nit)
    valImages.push_back(readImage<FloatImage3>(*nit));
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges, &assignDouble);
  int n = tree.size();
  PointMap3 rmap, cmap, bmap;
  PointLabelMap3 lmap;
  getPointMap(rmap, lmap, cmap, initSegImage, CRCONN3, false, true);
  getBorderPoints(bmap, initSegImage, false);
  std::list<flist> feats;
  for (int i = 0; i < n; ++i) {
    fTree::TreeNode const* pn = &(tree[i]);
    if (pn->child0 >= 0 && pn->child1 >= 0)
      merge(rmap, cmap, lmap, tree[pn->child0].label, 
	    tree[pn->child1].label, pn->label, true, false);
    Points3 const* pr = &(rmap.find(pn->label)->second);
    Points3 const* pc = &(cmap.find(pn->label)->second);
    PointMap3::const_iterator bit = bmap.find(pn->label);
    feats.push_back(flist());
    MaGraph mag(0);
    readMedialAxisGraph(mag, magFileNames.find(pn->label)->second);
    pruneMedialAxisGraph(mag, limbThreshold);
    smoothMedialAxisGraph(mag, 1.0);
    getRegionShapeFeatures(feats.back(), *pr, *pc, 
			   bit == bmap.end()? NULL: &(bit->second), mag);
    getRegionIntensityFeatures(feats.back(), *pc, valImages[IM_RAW]);
    getRegionIntensityFeatures(feats.back(), *pc, valImages[IM_PB]);
  }
  write2(featFileName, feats, false, ' ', '\n');
}



int main (int argc, char* argv[])
{
  if (argc < 9) {
    std::cerr << "Usage: " << argv[0] 
	      << " initSegImageName"
	      << " treeFileName"
	      << " rawImageName"
	      << " pbImageName"
	      << " {-g nodeLabel medialAxisGraphFileName}"
	      << " [limbThreshold]"
	      << " featureFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  std::vector<const char*> valImageNames;
  valImageNames.reserve(2);
  valImageNames.push_back(argv[argi++]);
  valImageNames.push_back(argv[argi++]);
  std::map<Label, const char*> magFileNames;
  while (strcmp(argv[argi], "-g") == 0) {
    Label l = atol(argv[argi + 1]);
    magFileNames[l] = argv[argi + 2];
    argi += 3;
  }
  double limbThreshold = argi < argc - 1? atof(argv[argi++]): 50.0;
  const char* featFileName = argv[argi++];
  operation(featFileName, initSegImageName, treeFileName, valImageNames, 
	    magFileNames, limbThreshold);
  return EXIT_SUCCESS;
}
