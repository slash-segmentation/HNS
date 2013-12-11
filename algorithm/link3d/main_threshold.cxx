#include "link3d_util.h"
#include "link3d_io.h"
using namespace n3;
using namespace n3::link3d;

typedef std::map<IntLabelPair, std::set<fLink> > LinkMap;

void pick (std::set<fLink>& picks, std::map<IntLabelPair, int>& counts, 
	   LinkMap& directs, LinkMap& skips, IntLabelPair const& profile, 
	   double dwth, double swth)
{
  bool found = false;
  // Direct
  LinkMap::iterator dit = directs.find(profile);
  if (dit != directs.end()) {
    std::list<std::set<fLink>::iterator> erm;
    for (std::set<fLink>::iterator it = dit->second.begin(); 
	 it != dit->second.end(); ++it)
      if (it->data >= dwth) {
	picks.insert(*it);
	erm.push_back(it);
	++counts[it->get_profile(0)];
	++counts[it->get_profile(1)];
	found = true;
      }
    remove(dit->second, erm);
  } 
  // Skip
  if (!found) {
    LinkMap::iterator sit = skips.find(profile);
    if (sit != skips.end()) {
      std::list<std::set<fLink>::iterator> erm;
      for (std::set<fLink>::iterator it = sit->second.begin(); 
	   it != sit->second.end(); ++it)
	if (it->data >= swth) {
	  picks.insert(*it);
	  erm.push_back(it);
	  ++counts[it->get_profile(0)];
	  ++counts[it->get_profile(1)];
	}
      remove(sit->second, erm);
    }
  }
}



void threshold (std::set<fLink>& picks, std::list<fLink> const& links, 
		double dwth, double swth, bool force)
{
  LinkMap fd, bd, fs, bs; // Forward/backward, direct/skipping
  std::map<IntLabelPair, int> counts;
  for (std::list<fLink>::const_iterator lit = links.begin(); 
       lit != links.end(); ++lit) {
    IntLabelPair profile0 = lit->get_profile(0);
    IntLabelPair profile1 = lit->get_profile(1);
    counts[profile0] = 0;
    counts[profile1] = 0;
    int sdiff = lit->s1 - lit->s0;
    if (sdiff == 1 || sdiff == -1) {
      fd[sdiff > 0? profile0: profile1].insert(*lit);
      bd[sdiff > 0? profile1: profile0].insert(*lit);
    }
    else {
      fs[sdiff > 0? profile0: profile1].insert(*lit);
      bs[sdiff > 0? profile1: profile0].insert(*lit);
    }
  }
  // For each direction pick/remove direct edges above dwth
  // Or skip edges above swth
  for (std::map<IntLabelPair, int>::const_iterator cit = counts.begin(); 
       cit != counts.end(); ++cit) {
    pick(picks, counts, fd, fs, cit->first, dwth, swth);
    pick(picks, counts, bd, bs, cit->first, dwth, swth);
  }
  if (force)
    // If no edge is picked force one direct connection
    for (std::map<IntLabelPair, int>::const_iterator cit = counts.begin(); 
	 cit != counts.end(); ++cit)
      if (cit->second == 0) {
	IntLabelPair profile = cit->first;
	fLink const* lmax = NULL;
	LinkMap::const_iterator fdit = fd.find(profile);
	if (fdit != fd.end())
	  for (std::set<fLink>::const_iterator it = fdit->second.begin(); 
	       it != fdit->second.end(); ++it) 
	    if (lmax == NULL || it->data > lmax->data) lmax = &(*it);
	LinkMap::const_iterator bdit = bd.find(profile);
	if (bdit != bd.end())
	  for (std::set<fLink>::const_iterator it = bdit->second.begin(); 
	       it != bdit->second.end(); ++it) 
	    if (lmax == NULL || it->data > lmax->data) lmax = &(*it);
	if (lmax != NULL) picks.insert(*lmax);
	// else perr("Unexpected: encounter region without any link...");
      }
}



void operation (std::map<int, const char*> const& outputImageNames, 
		std::map<int, const char*> const& inputImageNames, 
		std::vector<const char*> const& regionPairFileNames, 
		std::vector<const char*> const& weightFileNames, 
		double dwth, double swth, bool force, bool writeToUInt16)
{
  int n = regionPairFileNames.size();
  std::list<fLink> links;
  for (int i = 0; i < n; ++i) 
    readLinks(links, regionPairFileNames[i], weightFileNames[i]);
  std::set<fLink> picks;
  threshold(picks, links, dwth, swth, force);
  std::map<int, LabelImage::Pointer> inputImages, outputImages;
  for (std::map<int, const char*>::const_iterator it = 
	 inputImageNames.begin(); it != inputImageNames.end(); ++it)
    inputImages[it->first] = readImage<LabelImage>(it->second);
  link(outputImages, inputImages, picks);
  if (writeToUInt16)
    for (std::map<int, const char*>::const_iterator it = 
	   outputImageNames.begin(); it != outputImageNames.end(); ++it) {
      UInt16Image::Pointer outImage = 
	castImage<LabelImage, UInt16Image>(outputImages[it->first]);
      writeImage<UInt16Image>(it->second, outImage);
    }
  else 
    for (std::map<int, const char*>::const_iterator it = 
	   outputImageNames.begin(); it != outputImageNames.end(); ++it)
      writeImage<LabelImage>(it->second, outputImages[it->first]);
}



int main (int argc, char* argv[])
{
  if (argc < 10) {
    std::cerr << "Usage: " << argv[0] 
	      << " {-i sliceNo inputImageName outputImageName}"
	      << " {-l regionPairFileName weightFileName}"
	      << " directLinkThreshold"
	      << " skipLinkThreshold"
	      << " [writeToUInt16 = 0]"
	      << " [forceOneDirectLink = 1]"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  std::map<int, const char*> inputImageNames, outputImageNames;
  while (strcmp(argv[argi], "-i") == 0) {
    int sliceNo = atoi(argv[argi + 1]);
    inputImageNames.insert(std::make_pair(sliceNo, argv[argi + 2]));
    outputImageNames.insert(std::make_pair(sliceNo, argv[argi + 3]));
    argi += 4;
  }
  std::vector<const char*> regionPairFileNames, weightFileNames;
  int ln = (argc - inputImageNames.size() * 4 - 3) / 3 + 1;
  regionPairFileNames.reserve(ln);
  weightFileNames.reserve(ln);
  while (strcmp(argv[argi], "-l") == 0) {
    regionPairFileNames.push_back(argv[argi + 1]);
    weightFileNames.push_back(argv[argi + 2]);
    argi += 3;
  }
  double dwth = atof(argv[argi++]);
  double swth = atof(argv[argi++]);
  bool writeToUInt16 = 
    (argi < argc && atoi(argv[argi++]) != 0? true: false);
  bool force = argi < argc && atoi(argv[argi++]) == 0? false: true;
  operation(outputImageNames, inputImageNames, regionPairFileNames, 
	    weightFileNames, dwth, swth, force, writeToUInt16);
  return EXIT_SUCCESS;
}
