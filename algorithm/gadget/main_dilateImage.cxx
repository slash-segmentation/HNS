#include "util/struct_util.h"
#include "util/cv/cv_contour.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		bool relabel, bool writeToUInt16)
{
  LabelImage::Pointer image = readImage<LabelImage>(inputImageName);
  if (relabel) image = relabelImage<LabelImage, LabelImage>(image, 0);
  int width = getw<LabelImage>(image);
  int height = geth<LabelImage>(image);
  PointMap rmap;
  getPointMap(rmap, image, false);
  Points openSet;
  cv::Mat canvas(height, width, CV_8U);
  for (PointMap::const_reverse_iterator rit = rmap.rbegin(); 
       rit != rmap.rend(); ++rit) {
    Points contour;
    getInnerContour(contour, canvas, &(rit->second), CRCONN, width, 
		    height, 0, 0, false);
    splice(openSet, contour, false);
  }
  while (openSet.size() > 0) {
    Point p = openSet.front();
    openSet.pop_front();
    std::vector<Pixel<Label> > n = getNeighbors<LabelImage>(p, image, 4);
    for (std::vector<Pixel<Label> >::const_iterator nit = n.begin(); 
	 nit != n.end(); ++nit)
      if (nit->val == BGVAL) {
	openSet.push_back(*nit);
	setv<LabelImage>(image, nit->x, nit->y, 
			 getv<LabelImage>(image, p.x, p.y));
      }
  }
  if (writeToUInt16) 
    writeImage<UInt16Image>(outputImageName, 
			    castImage<LabelImage, UInt16Image>(image));
  else writeImage<LabelImage>(outputImageName, image);  
}



int main (int argc, char* argv[])
{
  if (argc != 5 && argc != 4) {
    std::cerr << "Usage: " << argv[0]
	      << " inputImageName" 
	      << " [relabel = 0" 
	      << " (1 to guarantee dilate smaller regions first)]"
	      << " [writeToUInt16 = 0]"
	      << " outputImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  bool relabel = (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false); 
  bool write16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false);
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, relabel, write16);
  return EXIT_SUCCESS;
}
