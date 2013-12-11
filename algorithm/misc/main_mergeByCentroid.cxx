#include "util/struct_util.h"
using namespace n3;

void operation (const char* outputImageName, const char* inputImageName, 
		std::vector<const char*> const& alignImageNames, 
		std::vector<const char*> const& refImageNames, 
		bool labelCC, bool write16)
{
  LabelImage::Pointer inputImage = readImage<LabelImage>(inputImageName);
  int n = alignImageNames.size();
  // Vote 
  std::map<LabelPair, int> votes;
  for (int i = 0; i < n; ++i) {
    LabelImage::Pointer alignImage = 
      readImage<LabelImage>(alignImageNames[i]);
    LabelImage::Pointer refImage = readImage<LabelImage>(refImageNames[i]);
    PointMap armap;
    getPointMap(armap, alignImage, false);
    std::map<Label, std::list<Label> > counts;
    for (PointMap::const_iterator rit = armap.begin(); rit != armap.end(); 
	 ++rit) {
      Point centroid = rit->second.get_centroid();
      Label ref = getv<LabelImage>(refImage, centroid.x, centroid.y);
      counts[ref].push_back(rit->first);
    }
    for (std::map<Label, std::list<Label> >::iterator cit = 
	   counts.begin(); cit != counts.end(); ++cit)
      if (cit->second.size() > 1) {
	cit->second.sort();
	cit->second.unique();
	for (std::list<Label>::const_iterator sit0 = cit->second.begin(); 
	     sit0 != cit->second.end(); ++sit0) {
	  std::list<Label>::const_iterator sit1 = sit0; ++sit1;
	  while (sit1 != cit->second.end()) {
	    LabelPair key = std::make_pair(*sit0, *sit1);
	    std::map<LabelPair, int>::iterator vit = votes.find(key);
	    if (vit != votes.end()) ++(vit->second);
	    else votes[key] = 1;
	    ++sit1;
	  }
	}
      }
  }
  // Group
  std::list<std::set<Label> > groups;
  for (std::map<LabelPair, int>::const_iterator vit = votes.begin(); 
       vit != votes.end(); ++vit) 
    if (vit->second >= n / 2 + 1) {
      Label r0 = vit->first.first, r1 = vit->first.second;
      std::list<std::set<Label> >::iterator git0 = groups.end();
      std::list<std::set<Label> >::iterator git1 = groups.end();
      for (std::list<std::set<Label> >::iterator git = groups.begin(); 
	   git != groups.end(); ++git) {
	if (git->count(r0) > 0) git0 = git;
	if (git->count(r1) > 0) git1 = git;
      }
      if (git0 == groups.end() && git1 == groups.end()) {
	std::set<Label> group;
	group.insert(r0);
	group.insert(r1);
	groups.push_back(group);
      }
      else if (git0 != groups.end() && git1 == groups.end())
	git0->insert(r1);
      else if (git0 == groups.end() && git1 != groups.end()) 
	git1->insert(r0);
      else if (git0 != git1) {
	git0->insert(git1->begin(), git1->end());
	groups.erase(git1);
      }
    }
  // Paint
  PointMap rmap;
  getPointMap(rmap, inputImage, true);
  PointLabelMap lmap;
  getPointNeighbors(lmap, rmap[BGVAL], inputImage, CRCONN);
  PointMap cmap;
  getNeighborPoints(cmap, lmap);
  Label labelToAssign = rmap.rbegin()->first + 1;
  std::list<PointMap::iterator> i2rm;
  for (std::list<std::set<Label> >::const_iterator git = groups.begin(); 
       git != groups.end(); ++git) {
    std::set<Label>::const_iterator lit0 = git->begin(); 
    std::set<Label>::const_iterator lit1 = lit0; ++lit1;
    Label r0 = *lit0;
    while (lit1 != git->end()) {
      Label r1 = *lit1;
      merge(rmap, cmap, lmap, r0, r1, labelToAssign);
      i2rm.push_back(rmap.find(r0));
      i2rm.push_back(rmap.find(r1));
      r0 = labelToAssign;
      ++labelToAssign;
      ++lit1;
    }
  }
  remove(rmap, i2rm);
  LabelImage::Pointer res = createImage<LabelImage>(inputImage, BGVAL);
  for (PointMap::const_iterator rit = rmap.begin(); rit != rmap.end(); 
       ++rit) setvs<LabelImage>(res, rit->second, rit->first);
  if (write16) {
    UInt16Image::Pointer out = labelCC? 
      labelConnectedComponents<LabelImage, UInt16Image>(res, BGVAL, false): 
      castImage<LabelImage, UInt16Image>(res);
    writeImage<UInt16Image>(outputImageName, out);
  }
  else {
    if (labelCC) 
      res = labelConnectedComponents<LabelImage, 
				     LabelImage>(res, BGVAL, false);
    writeImage<LabelImage>(outputImageName, res);
  }  
}



int main (int argc, char* argv[])
{
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputImageName" 
	      << " {-a alignedInputImageName}"
	      << " {-r referenceImageName}"
	      << " [labelConnectedComponents = 1]"
	      << " [writeToUInt16Image = 0]"
	      << " outputImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  std::vector<const char*> alignImageNames, refImageNames;
  alignImageNames.reserve((argc - 4) / 2 + 1);
  refImageNames.reserve((argc - 4) / 2 + 1);
  while (argi < argc - 1) {
    if (strcmp(argv[argi], "-a") == 0) {
      alignImageNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else if (strcmp(argv[argi], "-r") == 0) {
      refImageNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else break;
  }
  if (alignImageNames.size() != refImageNames.size()) 
    perr("Error: aligned and reference image number disagree...");
  bool labelCC = (argi < argc - 1 && atoi(argv[argi++]) == 0? false: true);
  bool write16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false);
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, alignImageNames, 
	    refImageNames, labelCC, write16);
  return EXIT_SUCCESS;
}
