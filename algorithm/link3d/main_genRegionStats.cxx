#include "rs.h"
#include "link3d_io.h"
using namespace n3;
using namespace n3::link3d;

void operation (const char* regionStatFileName, 
		const char* labelImageName, 
		const char* rawImageName, 
		const char* tdictFileName) 
{
  LabelImage::Pointer labelImage = readImage<LabelImage>(labelImageName);
  std::map<Label, RegionStat> smap;
  getRegionPoints(smap, labelImage);
  const int imageWidth = getw<LabelImage>(labelImage);
  const int imageHeight = geth<LabelImage>(labelImage);
  cv::Mat canvas(imageWidth + 2, imageHeight + 2, CV_8U);
  FloatImage::Pointer rawImage;
  TextonDict tdict;
  LabelImage::Pointer tcanvas;
  if (rawImageName != NULL) {
    rawImage = readImage<FloatImage>(rawImageName);
    readTexton(tdict, tdictFileName, TEXTON_PAT_LEN);
    tcanvas = createImage<LabelImage>(imageWidth, imageHeight, 0);
  }
  for (std::map<Label, RegionStat>::iterator sit = smap.begin(); 
       sit != smap.end(); ++sit) {
    RegionStat* rs = &sit->second;
    getContour(rs->ucontour, canvas, &rs->points, CRCONN, 
	       imageWidth, imageHeight, 1, 1, true);
    getRegionStat(*rs, canvas, imageWidth, imageHeight);
    if (rawImageName != NULL)
      getRegionStat(*rs, rawImage, tcanvas, tdict, 
		    0.0, 1.0, RS_RI_HIST_BIN);
  }
  writeRegionStats(regionStatFileName, smap);
}



int main (int argc, char* argv[])
{
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0] 
	      << " labelImageName" 
	      << " rawImageName ('NULL' to skip intensity/texture feats)"
	      << " textonDictFileName"
	      << " regionStatFileName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* rawImageName = 
    strcmp(argv[argi], "NULL") == 0? NULL: argv[argi];
  ++argi;
  const char* tdictFileName = argv[argi++];
  const char* regionStatFileName = argv[argi++];
  operation(regionStatFileName, labelImageName, rawImageName, 
	    tdictFileName);
  return EXIT_SUCCESS;
}
