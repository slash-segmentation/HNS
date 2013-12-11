#include "util/cv/cv_contour.h"
#include "tree2d_io.h"
#include "tree2d_util.h"
#include "tree_visit.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* contourFileName, const char* initSegImageName, 
		const char* treeFileName, const char* mergeProbFileName, 
		unsigned int z)
{
  LabelImage::Pointer initSegImage = 
    readImage<LabelImage>(initSegImageName);
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges);
  PointMap rmap, cmap;
  getPointMap(rmap, cmap, initSegImage, merges, false, CRCONN);
  std::list<double> mprobs;
  read(mprobs, mergeProbFileName, 1);
  getPotentials(tree, mprobs);
  std::list<int> picks;
  resolve(picks, tree);
  int width = getw<LabelImage>(initSegImage);
  int height = geth<LabelImage>(initSegImage);
  cv::Mat canvas(height + 2, width + 2, CV_8U);
  Label labelToAssign = 1;
  std::list<unsigned int> output;
  for (std::list<int>::const_iterator pit = picks.begin(); 
       pit != picks.end(); ++pit) {
    Label label = tree[*pit].label;
    std::vector<Points> contour;
    getContour(contour, canvas, &(rmap[label]), &(cmap[label]), 1, 1, true);
    for (std::vector<Points>::const_iterator cit = contour.begin(); 
	 cit != contour.end(); ++cit) {
      for (Points::const_iterator it = cit->begin(); 
	   it != cit->end(); ++it) {
	output.push_back(labelToAssign);
	output.push_back(it->x);
	output.push_back(it->y);
	output.push_back(z);
      }
      ++labelToAssign;
    }
  }
  write(contourFileName, output, 4, ' ');
}



int main (int argc, char* argv[])
{
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0] 
	      << " initialSegmentationImageName" 
	      << " treeFileName" 
	      << " mergeProbabilityFileName"
	      << " z"
	      << " contourFileName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  const char* mergeProbFileName = argv[argi++];
  unsigned int z = atoi(argv[argi++]);
  const char* contourFileName = argv[argi++];
  operation(contourFileName, initSegImageName, treeFileName, 
	    mergeProbFileName, z);
  return EXIT_SUCCESS;
}
