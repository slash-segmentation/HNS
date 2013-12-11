#include "link3d_io.h"
#include "link3d_util.h"
#include "bs.h"
using namespace n3;
using namespace n3::link3d;

void operation (const char* bodyFileName, 
		const char* bodyStatFileName, 
		std::map<int, const char*> const& labelImageNames2, 
		std::map<int, const char*> const& labelImageNames3, 
		float dz)
{
  std::map<int, LabelImage::Pointer> labelImages2, labelImages3;
  std::map<int, PointMap> rmaps;
  for (std::map<int, const char*>::const_iterator iit = 
	 labelImageNames2.begin(); iit != labelImageNames2.end(); ++iit) {
    LabelImage::Pointer i2 = readImage<LabelImage>(iit->second);
    labelImages2[iit->first] = i2; 
    getPointMap(rmaps[iit->first], i2, false);
  }
  for (std::map<int, const char*>::const_iterator iit = 
	 labelImageNames3.begin(); iit != labelImageNames3.end(); ++iit) 
    labelImages3[iit->first] = readImage<LabelImage>(iit->second);
  std::list<fBody> bodies;
  getBodies(bodies, rmaps, labelImages2, labelImages3, true); // Use skip
  writeBodies(bodyFileName, NULL, bodies);
  // Done generating bodies; start generating body stats
  int width = getw<LabelImage>(labelImages2.begin()->second);
  int height = geth<LabelImage>(labelImages2.begin()->second);
  cv::Mat canvas(height + 2, width + 2, CV_8U);
  std::map<Label, BodyStat> smap;  
  for (std::list<fBody>::const_iterator bit = bodies.begin(); 
       bit != bodies.end(); ++bit) 
    getBodyStat(smap[bit->label], *bit, dz, rmaps, canvas, width, height);
  writeBodyStats(bodyStatFileName, smap);
}



int main (int argc, char* argv[])
{
  if (argc < 8) {
    std::cerr << "Usage: " << argv[0] 
	      << " {-i sliceNo 2dSegImageName 3dSegImageName}"
	      << " relativeZResolution"
	      << " bodyFileName"
	      << " bodyStatFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  std::map<int, const char*> labelImageNames2, labelImageNames3;
  while (strcmp(argv[argi], "-i") == 0) {
    ++argi;
    int slice = atoi(argv[argi++]);
    labelImageNames2[slice] = argv[argi++];
    labelImageNames3[slice] = argv[argi++];
  }
  float dz = atof(argv[argi++]);
  const char* bodyFileName = argv[argi++];
  const char* bodyStatFileName = argv[argi++];
  operation(bodyFileName, bodyStatFileName, labelImageNames2, 
	    labelImageNames3, dz);
  return EXIT_SUCCESS;
}
