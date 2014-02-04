#include "tree2d/tree2d_io.h"
#include "bc.h"
#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

const int IM_NUM = 2;
const int IM_RAW = 0;
const int IM_PB = 1;

void operation (const char* featFileName, const char* labelImageName, 
		const char* treeFileName, const char* saliencyFileName, 
		std::vector<const char*> const& valImageNames, 
		const char* tdictFileName)
{
  LabelImage3::Pointer labelImage = readImage<LabelImage3>(labelImageName);
  std::vector<FloatImage3::Pointer> valImages;
  valImages.reserve(valImageNames.size());
  for (std::vector<const char*>::const_iterator nit = 
	 valImageNames.begin(); nit != valImageNames.end(); ++nit) 
    valImages.push_back(readImage<FloatImage3>(*nit));
  LabelImage3::Pointer canvas = createImage<LabelImage3>(labelImage, 0);
  TextonDict tdict;
  if (tdictFileName != NULL) 
    readTexton(tdict, tdictFileName, TEXTON_PAT_LEN3);
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, saliencyFileName);
  fTree tree;
  getTree(tree, merges, &assignDouble);
  updateLeafSaliencies(tree, 0.0);
  PointMap3 rmap, cmap;
  PointLabelMap3 lmap;
  getPointMap(rmap, lmap, cmap, labelImage, CRCONN3, false, true);
  // Use border points for geometry features!
  PointMap3 bmap;
  getBorderPoints(bmap, labelImage, false);
  std::list<flist> feats;
  int N = tree.size(), n = 1;
  for (fTree::const_iterator pa = tree.begin(); pa != tree.end(); ++pa) {
    if (pa->child0 >= 0 && pa->child1 >= 0) {
      fTree::TreeNode const* ch0 = &(tree[pa->child0]);
      fTree::TreeNode const* ch1 = &(tree[pa->child1]);
      Points3 const* r0 = &(rmap.find(ch0->label)->second);
      Points3 const* r1 = &(rmap.find(ch1->label)->second);
      Points3 const* uc0 = &(cmap.find(ch0->label)->second);
      Points3 const* uc1 = &(cmap.find(ch1->label)->second);
      PointMap3::const_iterator bit0 = bmap.find(ch0->label), 
	bit1 = bmap.find(ch1->label);
      Points3 ub;
      getBoundary(ub, *uc0, *uc1, canvas);
      bool swap01 = r0->size() > r1->size();
      feats.push_back(flist());
      getBoundaryUnorderedGeometryFeatures
	(feats.back(), r0, r1, uc0, uc1, &ub, 
	 bit0 == bmap.end()? NULL: &(bit0->second), 
	 bit1 == bmap.end()? NULL: &(bit1->second), swap01);
      if (valImageNames[IM_RAW] != NULL) 
	getBoundaryIntensityFeatures(feats.back(), ub, valImages[IM_RAW], 
				     0.0, 1.0, BC_RI_HIST_BIN);
      if (tdictFileName != NULL) 
	getBoundaryTextonFeatures(feats.back(), *r0, *r1, 
				  valImages[IM_RAW], tdict, canvas);
      if (valImageNames[IM_PB] != NULL) 
	getBoundaryIntensityFeatures(feats.back(), ub, valImages[IM_PB], 
				     0.0, 1.0, BC_PB_HIST_BIN);
      if (saliencyFileName != NULL)
	getSaliencyFeatures(feats.back(), ch0->data, ch1->data, 
			    pa->data, swap01);
      // Modify rmap, cmap and bmap -- not keep source
      merge(rmap, cmap, lmap, bmap, ch0->label, ch1->label, pa->label, 
	    true, false);
    }
    // // For display
    // if (n % 200 == 0 || n > N - 3)
    // 	std::cerr << featFileName << " (" << n << "/" << N << ")"
    // 		  << std::endl;
    // ++n;
    // // ~ For display
  }
  write2(featFileName, feats, false, ' ', '\n');
}



int main (int argc, char* argv[])
{
  if (argc != 8) {
    std::cerr << "Usage: " << argv[0] 
	      << " labelImageName" 
	      << " treeFileName"
	      << " saliencyFileName ('NULL' to skip saliency features)"
	      << " rawImageName ('NULL' to skip raw features)"
	      << " pbImageName ('NULL' to skip pb features)"
	      << " textonDictFileName ('NULL' to skip texton features)"
	      << " featureFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  const char* salFileName = 
    strcmp(argv[argi++], "NULL") == 0? NULL: argv[argi - 1];
  std::vector<const char*> valImageNames;
  valImageNames.reserve(IM_NUM);
  for (int i = 0; i < IM_NUM; ++i) 
    valImageNames.push_back(strcmp(argv[argi++], "NULL") == 0? 
			    NULL: argv[argi - 1]);
  const char* tdictFileName = 
    strcmp(argv[argi++], "NULL") == 0? NULL: argv[argi - 1];
  const char* featFileName = argv[argi++];
  operation(featFileName, labelImageName, treeFileName, salFileName, 
	    valImageNames, tdictFileName);
  return EXIT_SUCCESS;
}
