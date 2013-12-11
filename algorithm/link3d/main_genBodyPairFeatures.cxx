#include "link3d_io.h"
#include "sc.h"
#include "bc.h"
using namespace n3;
using namespace n3::link3d;

void operation (const char* featFileName, const char* bodyFileName, 
		const char* bodyStatFileName, const char* bodyPairFileName, 
		std::map<int, const char*> const& regionStatFileNames, 
		std::map<int, const char*> const& labelImageNames, 
		std::map<int, const char*> const& alignImageNames, 
		float dz)
{
  std::map<int, LabelImage::Pointer> labelImages, alignImages;
  std::map<int, PointMap> rmaps;
  std::map<int, std::map<Label, RegionStat> > rsmaps;
  for (std::map<int, const char*>::const_iterator nit = 
	 labelImageNames.begin(); nit != labelImageNames.end(); ++nit) {
    LabelImage::Pointer i2 = readImage<LabelImage>(nit->second);
    labelImages[nit->first] = i2;
    getPointMap(rmaps[nit->first], i2, false);
    readRegionStats(rsmaps[nit->first], 
		    regionStatFileNames.find(nit->first)->second);
  }
  std::map<int, PointMap> armaps, acmaps;
  for (std::map<int, const char*>::const_iterator nit = 
	 alignImageNames.begin(); nit != alignImageNames.end(); ++nit) { 
    LabelImage::Pointer im = readImage<LabelImage>(nit->second);
    alignImages[nit->first] = im;
    getPointMap(armaps[nit->first], im, false);
  }
  std::map<IntPair, std::map<LabelPair, unsigned int> > overlaps, aoverlaps;
  for (std::map<int, LabelImage::Pointer>::const_iterator iit0 = 
	 labelImages.begin(); iit0 != labelImages.end(); ++iit0) {
    std::map<int, LabelImage::Pointer>::const_iterator iit1 = next(iit0);
    IntPair key = std::make_pair(iit0->first, iit1->first);
    if (iit1 != labelImages.end())       
      getOverlaps(overlaps[key], iit0->second, iit1->second, 
		  false, false);
    std::map<int, LabelImage::Pointer>::const_iterator ait1 = 
      alignImages.find(iit1->first);
    if (ait1 != alignImages.end())
      getOverlaps(aoverlaps[key], iit0->second, ait1->second, 
		  false, false);
  }
  std::list<fBody> bodies;
  readBodies(bodies, bodyFileName, NULL);
  std::map<Label, fBody const*> pbs;
  for (std::list<fBody>::const_iterator bit = bodies.begin(); 
       bit != bodies.end(); ++bit) pbs[bit->label] = &(*bit);
  std::map<Label, BodyStat> bsmap;
  readBodyStats(bsmap, bodyStatFileName);
  std::list<fBodyLink> bpairs;
  readBodyLinks(bpairs, bodyPairFileName, NULL);
  int width = getw<LabelImage>(alignImages.begin()->second); 
  int height = geth<LabelImage>(alignImages.begin()->second);
  cv::Mat canvas(height + 2, width + 2, CV_8U);
  std::list<flist> feats;
  for (std::list<fBodyLink>::const_iterator pit = bpairs.begin(); 
       pit != bpairs.end(); ++pit) {
    feats.push_back(flist());
    // Body features
    Label l30 = pit->nodes[0].first, l31 = pit->nodes[1].first;
    Label id0 = pit->nodes[0].second, id1 = pit->nodes[1].second;
    fBody::Node const* node0 = &(pbs[l30]->nodes.find(id0)->second);
    fBody::Node const* node1 = &(pbs[l31]->nodes.find(id1)->second);
    Label l20 = node0->data.label2, l21 = node1->data.label2;
    int slice0 = node0->data.slice, slice1 = node1->data.slice;
    BodyStat* bs0 = &(bsmap.find(l30)->second);
    BodyStat* bs1 = &(bsmap.find(l31)->second);
    // getAdvancedGeometry(feats.back(), id0, id1, rmaps, *bs0, *bs1, 
    // 			*(pbs[l30]), *(pbs[l31]), dz, canvas, 
    // 			width, height);
    // Region features
    IntPair key = std::make_pair(slice0, slice1);
    std::map<int, std::map<Label, RegionStat> >::const_iterator 
      rsmit0 = rsmaps.find(slice0), rsmit1 = rsmaps.find(slice1);
    if (rsmit0 == rsmaps.end() || rsmit1 == rsmaps.end()) 
      perr("Unexpected: cannot find region stat map...");
    std::map<Label, RegionStat>::const_iterator 
      rsit0 = rsmit0->second.find(l20), rsit1 = rsmit1->second.find(l21);
    if (rsit0 == rsmit0->second.end() || rsit1 == rsmit1->second.end())
      perr("Unexpected: cannot find region stat...");
    RegionStat const* rs0 = &(rsit0->second);
    RegionStat const* rs1 = &(rsit1->second);
    double a0 = rs0->stats[0][0], a1 = rs1->stats[0][0];
    double ap0 = rs0->ucontour.size(), ap1 = rs1->ucontour.size();
    bool swap01 = a0 > a1;
    std::map<IntPair, std::map<LabelPair, unsigned int> >::const_iterator 
      ovit = overlaps.find(key);
    if (ovit == overlaps.end()) 
      perr("Unexpected: cannot find overlap map...");
    std::map<LabelPair, unsigned int>::const_iterator oit = 
      ovit->second.find(std::make_pair(l20, l21));
    getSimpleGeometry(feats.back(), a0, a1, ap0, ap1, 
		      rs0->centroid.get_distance(rs1->centroid), 
		      oit == ovit->second.end()? 0.0: oit->second, swap01);
    getAdvancedGeometry(feats.back(), rs0, rs1, swap01);
    if (alignImages.size() > 0) {
      std::map<int, PointMap>::const_iterator armit = armaps.find(slice1);
      PointMap::const_iterator arit = armit->second.find(l21);
      std::map<IntPair, std::map<LabelPair, unsigned int> >::const_iterator 
	aovit = aoverlaps.find(key);
      if (aovit == aoverlaps.end()) 
	perr("Unexpected: cannot find aligned overlap map...");
      double aa1 = 0.0, aap1 = 0.0, ao = 0.0, acd = DUMMY;
      if (arit != armit->second.end()) {
	Points acontour; 
	getInnerContour(acontour, canvas, &(arit->second), CRCONN, width, 
			height, 1, 1, true);
	aa1 = arit->second.size();
	aap1 = acontour.size();
	std::map<LabelPair, unsigned int>::const_iterator aoit = 
	  aovit->second.find(std::make_pair(l20, l21));
	ao = aoit == aovit->second.end()? 0.0: aoit->second;
	acd = arit->second.get_centroid().get_distance(rs0->centroid);
      }
      getSimpleGeometry(feats.back(), a0, aa1, ap0, aap1, acd, ao, swap01);
    }
  }
  write2(featFileName, feats, false, ' ', '\n');
}



int main (int argc, char* argv[])
{
  if (argc < 10) {
    std::cerr << "Usage: " << argv[0] 
	      << " bodyFileName"
	      << " bodyStatFileName"
	      << " bodyPairFileName"
	      << " {-i sliceNo 2dSegImageName regionStatFileName}"
	      << " [{-a sliceNo aligned2DSegImageName}]"
	      << " relativeZResolution"
	      << " featFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* bodyFileName = argv[argi++];
  const char* bodyStatFileName = argv[argi++];
  const char* bodyPairFileName = argv[argi++];
  std::map<int, const char*> labelImageNames, regionStatFileNames;
  while (strcmp(argv[argi], "-i") == 0) {
    ++argi;
    int sliceNo = atoi(argv[argi++]);
    labelImageNames[sliceNo] = argv[argi++];
    regionStatFileNames[sliceNo] = argv[argi++];    
  }
  std::map<int, const char*> alignImageNames;
  while (strcmp(argv[argi], "-a") == 0) {
    ++argi;
    int sliceNo = atoi(argv[argi++]);
    alignImageNames[sliceNo] = argv[argi++];
  }
  float dz = atof(argv[argi++]);
  const char* featFileName = argv[argi++];
  operation(featFileName, bodyFileName, bodyStatFileName, 
	    bodyPairFileName, regionStatFileNames, labelImageNames, 
	    alignImageNames, dz);
  return EXIT_SUCCESS;
}
