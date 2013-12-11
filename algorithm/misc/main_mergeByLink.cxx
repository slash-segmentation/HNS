#include "util/struct_util.h"
#include "link3d/link3d_io.h"
using namespace n3;
using namespace n3::link3d;

void vote (std::map<LabelPair, int>& votes, const char* regionPairFileName, 
	   const char* weightFileName, double threshold, bool isFwd)
{
  std::list<fLink> links;
  readLinks(links, regionPairFileName, weightFileName);
  std::map<Label, std::list<Label> > collects;
  if (isFwd) {
    for (std::list<fLink>::const_iterator lit = links.begin(); 
	 lit != links.end(); ++lit)
      if (lit->data >= threshold) collects[lit->r1].push_back(lit->r0);
  }
  else {
    for (std::list<fLink>::const_iterator lit = links.begin(); 
	 lit != links.end(); ++lit) 
      if (lit->data >= threshold) collects[lit->r0].push_back(lit->r1);
  }
  for (std::map<Label, std::list<Label> >::iterator cit = 
	 collects.begin(); cit != collects.end(); ++cit) {
    if (cit->second.size() > 1) {
      if (cit->second.size() > 2) {
	cit->second.sort();
	cit->second.unique();
      }
      for (std::list<Label>::const_iterator lit0 = cit->second.begin(); 
	   lit0 != cit->second.end(); ++lit0) {
	std::list<Label>::const_iterator lit1 = lit0; ++lit1;
	while (lit1 != cit->second.end()) {
	  LabelPair key = std::make_pair(*lit0, *lit1);
	  std::map<LabelPair, int>::iterator vit = votes.find(key);
	  if (vit != votes.end()) ++(vit->second);
	  else votes[key] = 1;
	  ++lit1;
	}
      }
    }
  }
}



void operation (const char* outputImageName, const char* inputImageName, 
		std::vector<const char*> const& fwdRegionPairFileNames, 
		std::vector<const char*> const& bwdRegionPairFileNames, 
		std::vector<const char*> const& fwdWeightFileNames, 
		std::vector<const char*> const& bwdWeightFileNames, 
		std::vector<double> const& fwdThresholds, 
		std::vector<double> const& bwdThresholds, 
		bool labelCC, bool write16)
{
  LabelImage::Pointer inputImage = readImage<LabelImage>(inputImageName);
  int nf = fwdThresholds.size(), nb = bwdThresholds.size();
  // Vote
  std::map<LabelPair, int> votes;
  for (int i = 0; i < nf; ++i)
    vote(votes, fwdRegionPairFileNames[i], fwdWeightFileNames[i], 
	 fwdThresholds[i], true);
  for (int i = 0; i < nb; ++i) 
    vote(votes, bwdRegionPairFileNames[i], bwdWeightFileNames[i], 
	 bwdThresholds[i], false);
  // Group
  std::list<std::set<Label> > groups;
  for (std::map<LabelPair, int>::const_iterator vit = votes.begin(); 
       vit != votes.end(); ++vit) 
    if (vit->second >= (nf + nb) / 2 + 1) {
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
  if (argc < 9) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputImageName" 
	      << " {-fp forwardRegionPairFileName}"
	      << " {-fw forwardWeightFileName}"
	      << " {-ft forwardWeightThreshold}"
	      << " {-bp backwardRegionPairFileName}"
	      << " {-bw backwardWeightFileName}"
	      << " {-bt backwardWeightThreshold}"
	      << " [labelConnectedComponent = 1]"
	      << " [writeToUInt16Image = 0]"
	      << " outputImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputImageName = argv[argi++];
  std::vector<const char*> fwdRegionPairFileNames, bwdRegionPairFileNames;
  std::vector<const char*> fwdWeightFileNames, bwdWeightFileNames;
  std::vector<double> fwdThresholds, bwdThresholds;
  fwdRegionPairFileNames.reserve((argc - 3) / 6 + 1);
  bwdRegionPairFileNames.reserve((argc - 3) / 6 + 1);
  fwdWeightFileNames.reserve((argc - 3) / 6 + 1);
  bwdWeightFileNames.reserve((argc - 3) / 6 + 1);
  fwdThresholds.reserve((argc - 3) / 6 + 1);
  bwdThresholds.reserve((argc - 3) / 6 + 1);
  while (argi < argc - 1) {
    if (strcmp(argv[argi], "-fp") == 0) {
      fwdRegionPairFileNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else if (strcmp(argv[argi], "-fw") == 0) {
      fwdWeightFileNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else if (strcmp(argv[argi], "-ft") == 0) {
      fwdThresholds.push_back(atof(argv[argi + 1]));
      argi += 2;
    }
    else if (strcmp(argv[argi], "-bp") == 0) {
      bwdRegionPairFileNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else if (strcmp(argv[argi], "-bw") == 0) {
      bwdWeightFileNames.push_back(argv[argi + 1]);
      argi += 2;
    }
    else if (strcmp(argv[argi], "-bt") == 0) {
      bwdThresholds.push_back(atof(argv[argi + 1]));
      argi += 2;
    }
    else break;
  }
  if (fwdRegionPairFileNames.size() != fwdWeightFileNames.size() || 
      fwdRegionPairFileNames.size() != fwdThresholds.size() || 
      bwdRegionPairFileNames.size() != bwdWeightFileNames.size() || 
      bwdRegionPairFileNames.size() != bwdThresholds.size())
    perr("Error: region pair/weight/threshold file number disagree...");
  bool labelCC = (argi < argc - 1 && atoi(argv[argi++]) == 0? false: true);
  bool write16 = (argi < argc - 1 && atoi(argv[argi++]) != 0? true: false);
  const char* outputImageName = argv[argi++];
  operation(outputImageName, inputImageName, fwdRegionPairFileNames, 
	    bwdRegionPairFileNames, fwdWeightFileNames, 
	    bwdWeightFileNames, fwdThresholds, bwdThresholds, 
	    labelCC, write16);
  return EXIT_SUCCESS;
}
