#include "link3d_io.h"
#include "sc.h"
using namespace n3;
using namespace n3::link3d;

// Always project (align) image 1 to image 0
void operation (const char* featFileName, const char* regionPairFileName, 
		const char* labelImageName0, const char* labelImageName1, 
		const char* alignImageName1, const char* rstatFileName0, 
		const char* rstatFileName1, bool useIntensityFeature)
{
  LabelImage::Pointer labelImage0 = readImage<LabelImage>(labelImageName0);
  LabelImage::Pointer labelImage1 = readImage<LabelImage>(labelImageName1);
  std::list<fLink> links;
  readLinks(links, regionPairFileName, NULL);
  std::map<Label, RegionStat> smap0, smap1;
  readRegionStats(smap0, rstatFileName0);
  readRegionStats(smap1, rstatFileName1);
  std::map<LabelPair, unsigned int> overlaps;
  getOverlaps(overlaps, labelImage0, labelImage1, false, false);
  LabelImage::Pointer alignImage1;
  std::map<LabelPair, unsigned int> aoverlaps;
  PointMap armap, acmap;
  std::map<Label, Point> acentroids;
  if (alignImageName1 != NULL) {
    alignImage1 = readImage<LabelImage>(alignImageName1);
    getPointMap(armap, alignImage1, false);
    alignImage1 = dilateImage<LabelImage>(alignImage1, armap);
    int w = getw<LabelImage>(alignImage1);
    int h = geth<LabelImage>(alignImage1);
    cv::Mat canvas(h, w, CV_8U);
    getOverlaps(aoverlaps, labelImage0, alignImage1, false, false);
    for (PointMap::const_iterator rit = armap.begin(); rit != armap.end(); 
	 ++rit) {
      acentroids[rit->first] = rit->second.get_centroid();
      getInnerContour(acmap[rit->first], canvas, &(rit->second), CRCONN, 
		      w, h, 0, 0, true);
    }
  }
  std::list<flist> feats;
  for (std::list<fLink>::const_iterator pit = links.begin(); 
       pit != links.end(); ++pit) {
    std::map<Label, RegionStat>::const_iterator rs0 = smap0.find(pit->r0);
    std::map<Label, RegionStat>::const_iterator rs1 = smap1.find(pit->r1);
    std::map<LabelPair, unsigned int>::const_iterator oit = 
      overlaps.find(pit->get_region_pair());
    if (rs0 != smap0.end() && rs1 != smap1.end()) {
      RegionStat const* prs0 = &(rs0->second);
      RegionStat const* prs1 = &(rs1->second);
      double a0 = prs0->stats[0][0], a1 = prs1->stats[0][0];
      double ap0 = prs0->ucontour.size(), ap1 = prs1->ucontour.size();
      bool swap01 = a0 > a1;
      feats.push_back(flist());
      getSimpleGeometry(feats.back(), a0, a1, ap0, ap1, 
      			prs0->centroid.get_distance(prs1->centroid), 
      			oit == overlaps.end()? 0: oit->second, swap01);
      getAdvancedGeometry(feats.back(), prs0, prs1, swap01);
      if (useIntensityFeature) {
	getIntensity(feats.back(), prs0, prs1, swap01);
	getTexture(feats.back(), prs0, prs1);
      }
      if (alignImageName1 != NULL) {
	double aa1 = 0.0, aap1 = 0.0, ao = 0.0, acd = DUMMY;
	PointMap::const_iterator arit = armap.find(pit->r1);
	PointMap::const_iterator acit = acmap.find(pit->r1);
	std::map<LabelPair, unsigned int>::const_iterator aoit = 
	  aoverlaps.find(pit->get_region_pair());
	std::map<Label, Point>::const_iterator acdit = 
	  acentroids.find(pit->r1);
	if (arit != armap.end()) aa1 = arit->second.size();
	if (acit != acmap.end()) aap1 = acit->second.size();
	if (aoit != aoverlaps.end()) ao = aoit->second;	
	if (acdit != acentroids.end()) 
	  acd = acdit->second.get_distance(prs0->centroid);
	getSimpleGeometry(feats.back(), a0, aa1, ap0, aap1, acd, ao, 
			  swap01);
      }
    }
    else perr("Unexpected: region pair/stats do not match...");
  }
  write2(featFileName, feats, false, ' ', '\n');
}



int main (int argc, char* argv[])
{
  if (argc != 8 && argc != 9) {
    std::cerr << "Usage: " << argv[0]
	      << " regionPairFileName"
	      << " labelImageName0"
	      << " labelImageName1"
	      << " alignImageName1 ('NULL' to skip)"
	      << " regionStatFileName0"
	      << " regionStatFileName1"
	      << " [useIntensityFeature = 0]"
	      << " featFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* regionPairFileName = argv[argi++];
  const char* labelImageName0 = argv[argi++];
  const char* labelImageName1 = argv[argi++];
  const char* alignImageName1 = 
    strcmp(argv[argi++], "NULL") == 0? NULL: argv[argi - 1];
  const char* rstatFileName0 = argv[argi++];
  const char* rstatFileName1 = argv[argi++];
  bool useIntensityFeature = 
    argi < argc - 1 && atoi(argv[argi++]) != 0? true: false;
  const char* featFileName = argv[argi++];
  operation(featFileName, regionPairFileName, labelImageName0, 
	    labelImageName1, alignImageName1, rstatFileName0, 
	    rstatFileName1, useIntensityFeature);
  return EXIT_SUCCESS;
}
