#include "util/image_util.h"
#include "util/metric/metric_pairf.h"
#include "tree2d_util.h"
using namespace n3;
using namespace n3::tree2d;

int getLabel (PointMap& rmap, PointMap& cmap, PointLabelMap& lmap, 
	      Label r0, Label r1, Label r01, LabelImage::Pointer canvas, 
	      LabelImage::Pointer truthImage)
{
  std::list<const Points*> sub, sup;
  sub.push_back(&(rmap.find(r0)->second));
  sub.push_back(&(rmap.find(r1)->second));
  Points ub;
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



void operation (std::vector<const char*> const& pbImageNames, 
		std::vector<const char*> const& truthImageNames, 
		float threshold, int ath0, int ath1, double pth)
{
  int n = pbImageNames.size();
  std::vector<FloatImage::Pointer> pbImages(n);
  std::vector<LabelImage::Pointer> truthImages(n), labelImages(n);
  std::vector<BigInt> tps(n), tns(n), fps(n), fns(n);
  std::vector<int> regionNums(n, 0);
// #pragma omp parallel for
  for (int i = 0; i < n; ++i) {
    pbImages[i] = readImage<FloatImage>(pbImageNames[i]);
    truthImages[i] = readImage<LabelImage>(truthImageNames[i]);
    labelImages[i] = 
      watershed<FloatImage, LabelImage>(pbImages[i], threshold, 
					true, true);
    labelImages[i] = merge(labelImages[i], pbImages[i], ath0, ath1, pth);
    getPairScores(tps[i], tns[i], fps[i], fns[i], labelImages[i], 
		  truthImages[i]);
    std::set<Label> vals;
    getUniqueValues<LabelImage>(vals, labelImages[i]);
    regionNums[i] = vals.size();
  }
  BigInt tp = 0, fp = 0, fn = 0;
  int regionNum = 0;
  for (int i = 0; i < n; ++i) {
    tp += tps[i];
    fp += fps[i];
    fn += fns[i];
    regionNum += regionNums[i];
  }
  BigFloat prec = getPrecision(tp, fp), rec = getRecall(tp, fn);
  double regionNumMean = getMean(regionNums);
  double regionNumStd = getStd(regionNums, regionNumMean);
  std::cout << "Precision = " << prec << std::endl;
  std::cout << "Recall = " << rec << std::endl;
  std::cout << "Region # mean = " << regionNumMean << std::endl;
  std::cout << "Region # std. dev. = " << regionNumStd << std::endl;
  std::vector<int> posSampleNums(n, 0), negSampleNums(n, 0);
// #pragma omp parallel for
  for (int i = 0; i < n; ++i) {
    std::list<fMerge> merges;
    getMerges(merges, labelImages[i], pbImages[i]);
    fTree tree;
    getTree(tree, merges);
    PointMap rmap, cmap;
    PointLabelMap lmap;
    getPointMap(rmap, lmap, cmap, labelImages[i], CRCONN, false, true);
    LabelImage::Pointer canvas = 
      createImage<LabelImage>(labelImages[i], 0);
    std::list<int> labels;
    for (fTree::const_iterator pa = tree.begin(); pa != tree.end(); ++pa)
      if (pa->child0 >= 0 && pa->child1 >= 0) {
	int l = getLabel(rmap, cmap, lmap, tree[pa->child0].label, 
			 tree[pa->child1].label, pa->label, canvas, 
			 truthImages[i]);
	l > 0? ++posSampleNums[i]: ++negSampleNums[i];
      }
  }
  int posSampleNum = 0, negSampleNum = 0;
  for (int i = 0; i < n; ++i) {
    posSampleNum += posSampleNums[i];
    negSampleNum += negSampleNums[i];
  }
  std::cout << "Merge/split ratio = " << getr(negSampleNum, posSampleNum) 
	    << std::endl;
}



int main (int argc, char* argv[])
{
  if (argc < 7 || argc > 10) {
    std::cerr << "Usage: " << argv[0] 
	      << " imageNum"
	      << " startSliceNo"
	      << " sliceNoInc"
	      << " pbImageNameTemplate"
	      << " truthImageNameTemplate"
	      << " watershedThreshold"
	      << " [areaThreshold0 = 50]"
	      << " [areaThreshold1 = 200]"
	      << " [pbThreshold = 0.5]"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  int n = atoi(argv[argi++]);
  int startSliceNo = atoi(argv[argi++]);
  int sliceNoInc = atoi(argv[argi++]);
  const char* pbImageNameTemplate = argv[argi++];
  const char* truthImageNameTemplate = argv[argi++];
  float threshold = atof(argv[argi++]);
  int ath0 = argi < argc? atoi(argv[argi++]): 50;
  int ath1 = argi < argc? atoi(argv[argi++]): 200;
  double pth = argi < argc? atof(argv[argi++]): 0.5;
  std::vector<const char*> pbImageNames, truthImageNames;
  pbImageNames.reserve(n);
  truthImageNames.reserve(n);
  for (int i = 0; i < n; ++i) {
    char* pn = new char[1000];
    sprintf(pn, pbImageNameTemplate, startSliceNo + i * sliceNoInc);
    pbImageNames.push_back(pn);
    pn = new char[1000];
    sprintf(pn, truthImageNameTemplate, startSliceNo + i * sliceNoInc);
    truthImageNames.push_back(pn);
  }
  operation(pbImageNames, truthImageNames, threshold, ath0, ath1, pth);
  for (int i = 0; i < n; ++i) {
    delete[] pbImageNames[i];
    delete[] truthImageNames[i];
  }
  return EXIT_SUCCESS;
}
