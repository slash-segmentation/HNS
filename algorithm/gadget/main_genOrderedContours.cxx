#include "n3_io.h"
#include "util/cv/cv_contour.h"
#include "util/struct_util.h"
using namespace n3;

void operation (const char* contourFileName, const char* labelImageName, 
		unsigned int z)
{
  LabelImage::Pointer im = readImage<LabelImage>(labelImageName);
  PointMap rmap, cmap;
  PointLabelMap lmap;
  getPointMap(rmap, lmap, cmap, im, CRCONN, false, false);
  int w = getw<LabelImage>(im), h = geth<LabelImage>(im);
  cv::Mat canvas(h + 2, w + 2, CV_8U);
  Label labelToAssign = 1;
  std::list<unsigned int> output;
  for (PointMap::const_iterator rit = rmap.begin(); rit != rmap.end(); 
       ++rit) {
    PointMap::const_iterator cit = cmap.find(rit->first);
    if (cit != cmap.end()) {
      std::vector<Points> c;
      getContour(c, canvas, &(rit->second), &(cit->second), 
		 1, 1, true);
      for (std::vector<Points>::const_iterator vit = c.begin(); 
	   vit != c.end(); ++vit) {
	for (Points::const_iterator pit = vit->begin(); 
	     pit != vit->end(); ++pit) {
	  output.push_back(labelToAssign);
	  output.push_back(pit->x);
	  output.push_back(pit->y);
	  output.push_back(z);
	}
	++labelToAssign;
      }
    }
    else perr("Unexpected: cannot find region contour...");
  }
  write(contourFileName, output, 4, ' ');
}



int main (int argc, char* argv[])
{
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0]
	      << " labelImageName"
	      << " z"
	      << " contourFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  unsigned int z = atol(argv[argi++]);
  const char* contourFileName = argv[argi++];
  operation(contourFileName, labelImageName, z);
  return EXIT_SUCCESS;
}
