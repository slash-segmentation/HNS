#include "rs.h"
#include "link3d_io.h"
using namespace n3;
using namespace n3::link3d;

void operation (const char* regionStatFileName, 
		const char* labelImageName) 
{
  LabelImage::Pointer labelImage = readImage<LabelImage>(labelImageName);
  std::map<Label, RegionStat> smap;
  getRegionPoints(smap, labelImage);
  const int imageWidth = getw<LabelImage>(labelImage);
  const int imageHeight = geth<LabelImage>(labelImage);
  cv::Mat canvas(imageWidth + 2, imageHeight + 2, CV_8U);
  for (std::map<Label, RegionStat>::iterator sit = smap.begin(); 
       sit != smap.end(); ++sit) {
    RegionStat* rs = &sit->second;
    getContour(rs->ucontour, canvas, &rs->points, CRCONN, 
	       imageWidth, imageHeight, 1, 1, true);
    getRegionStat(*rs, canvas, imageWidth, imageHeight);
  }
  writeRegionStats(regionStatFileName, smap);
}



int main (int argc, char* argv[])
{
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] 
	      << " labelImageName" 
	      << " regionStatFileName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* regionStatFileName = argv[argi++];
  operation(regionStatFileName, labelImageName);
  return EXIT_SUCCESS;
}
