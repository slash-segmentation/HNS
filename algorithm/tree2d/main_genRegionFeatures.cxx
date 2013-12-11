#include "rc.h"
#include "tree2d_io.h"
#include "tree2d_util.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* featFileName, const char* labelImageName, 
		const char* treeFileName, const char* rawImageName)
{
  LabelImage::Pointer labelImage = readImage<LabelImage>(labelImageName);
  FloatImage::Pointer rawImage;
  if (rawImageName != NULL) rawImage = readImage<FloatImage>(rawImageName);
  int width = getw<LabelImage>(labelImage);
  int height = geth<LabelImage>(labelImage);
  cv::Mat canvas(height + 2, width + 2, CV_8U);
  std::list<flist> feats;
  if (treeFileName != NULL) {
    std::list<fMerge> merges;
    readMerges(merges, treeFileName, NULL);
    PointMap rmap, cmap;
    getPointMap(rmap, cmap, labelImage, merges, false, CRCONN);
    fTree tree;
    getTree(tree, merges, &(assignDouble));
    for (fTree::const_iterator tit = tree.begin(); tit != tree.end(); 
	 ++tit) {
      feats.push_back(flist());
      genRegionFeature(feats.back(), rmap[tit->label], cmap[tit->label], 
		       rawImage, canvas, rawImageName != NULL);
    }
  }
  else {
    PointMap rmap;
    getPointMap(rmap, labelImage, false);
    for (PointMap::const_iterator rit = rmap.begin(); rit != rmap.end(); 
	 ++rit) {
      Points contour;
      getContour(contour, canvas, &(rit->second), CRCONN, width, height, 
		 1, 1, false);
      feats.push_back(flist());
      genRegionFeature(feats.back(), rit->second, contour, rawImage, 
		       canvas, rawImageName != NULL);
    }
  }
  write2(featFileName, feats, false, ' ', '\n');
}



int main (int argc, char* argv[])
{
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0] 
	      << " labelImageName"
	      << " treeFileName ('NULL' to skip)" 
	      << " rawImageName ('NULL' to skip)" 
	      << " featFileName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* treeFileName = 
    strcmp(argv[argi++], "NULL") == 0? NULL: argv[argi - 1];
  const char* rawImageName = 
    strcmp(argv[argi++], "NULL") == 0? NULL: argv[argi - 1];
  const char* featFileName = argv[argi++];
  operation(featFileName, labelImageName, treeFileName, rawImageName);
  return EXIT_SUCCESS;
}
