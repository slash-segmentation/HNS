#include "util/metric/metric_pairf.h"
#include "tree2d/tree2d_io.h"
#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

int getLabel (PointMap3& rmap, PointMap3& cmap, PointLabelMap3& lmap, 
	      Label r0, Label r1, Label r01, LabelImage3::Pointer canvas, 
	      LabelImage3::Pointer truthImage)
{
  std::list<const Points3*> sub, sup;
  sub.push_back(&(rmap.find(r0)->second));
  sub.push_back(&(rmap.find(r1)->second));
  Points3 ub;
  getBoundary(ub, cmap.find(r0)->second, cmap.find(r1)->second, canvas);
  std::vector<BigFloat> subScores = getPairScores(sub, &ub, truthImage);
  merge(rmap, cmap, lmap, r0, r1, r01, true, false);
  sup.push_back(&(rmap.find(r01)->second));
  std::vector<BigFloat> supScores = getPairScores(sup, &ub, truthImage);
  // Boundary is true; do not merge
  int ret = 1;
  // Boundary is false; merge
  if (subScores[2] < supScores[2] || 
      (subScores[0] < EPSILON && subScores[1] < EPSILON && 
       supScores[0] < EPSILON && supScores[1] < EPSILON) || 
      (subScores[2] == supScores[2] && subScores[0] > 0.9 && 
       supScores[0] > 0.9)) ret = -1;
  return ret;
}


void operation (const char* labelFileName, const char* labelImageName, 
		const char* treeFileName, const char* truthImageName)
{
  LabelImage3::Pointer truthImage = readImage<LabelImage3>(truthImageName);
  LabelImage3::Pointer labelImage = readImage<LabelImage3>(labelImageName);
  LabelImage3::Pointer canvas = createImage<LabelImage3>(labelImage, 0);
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges);
  PointMap3 rmap, cmap;
  PointLabelMap3 lmap;
  getPointMap(rmap, lmap, cmap, labelImage, CRCONN3, false, true);
  std::list<int> labels;
  int N = tree.size(), n = 1;
  for (fTree::const_iterator pa = tree.begin(); pa != tree.end(); ++pa) {
    if (pa->child0 >= 0 && pa->child1 >= 0) {
      // For debug
      time_t local = time(0);
      // ~ For debug
      labels.push_back(getLabel(rmap, cmap, lmap, tree[pa->child0].label, 
				tree[pa->child1].label, pa->label, canvas, 
				truthImage));
      // For debug
      std::cerr << "generating label (" << tree[pa->child0].label << ", " 
		<< tree[pa->child1].label << ") took " 
		<< difftime(time(0), local) 
		<< " [" << n << "/" << N << "]" << std::endl;
      local = time(0);
      // ~ For debug
    }
    ++n;
  }
  write(labelFileName, labels, 1);
}



int main (int argc, char* argv[])
{
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
	      << " labelImageName"
	      << " treeFileName"
	      << " truthImageName"
	      << " labelFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* treeFileName = 
    strcmp(argv[argi++], "NULL") == 0? NULL: argv[argi - 1];
  const char* truthImageName = argv[argi++];
  const char* labelFileName = argv[argi++];
  operation(labelFileName, labelImageName, treeFileName, truthImageName);
  return EXIT_SUCCESS;
}
