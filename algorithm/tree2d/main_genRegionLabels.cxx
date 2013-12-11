#include "util/metric/metric_util.h"
#include "tree2d_io.h"
#include "tree2d_util.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* labelFileName, const char* initSegImageName, 
		const char* treeFileName, const char* truthImageName, 
		double overlapRatioThreshold)
{
  LabelImage::Pointer initSegImage = 
    readImage<LabelImage>(initSegImageName);
  LabelImage::Pointer truthImage = readImage<LabelImage>(truthImageName);
  std::list<fMerge> merges;
  readMerges(merges, treeFileName, NULL);
  fTree tree;
  getTree(tree, merges);
  PointMap rmap, cmap;
  getPointMap(rmap, cmap, initSegImage, merges, false, CRCONN);
  // Find out seg-to-truth overlaps and then find truth-to-seg overlaps
  std::map<Label, std::map<Label, unsigned int> > stOverlaps, tsOverlaps;
  for (PointMap::const_iterator rit = rmap.begin(); 
       rit != rmap.end(); ++rit) 
    getOverlaps(stOverlaps[rit->first], rit->second, truthImage, false);
  for (std::map<Label, std::map<Label, unsigned int> >::const_iterator 
	 stoit = stOverlaps.begin(); stoit != stOverlaps.end(); ++stoit) {
    Label seg = stoit->first;
    for (std::map<Label, unsigned int>::const_iterator oit = 
	   stoit->second.begin(); oit != stoit->second.end(); ++oit) {
      Label truth = oit->first;
      unsigned int overlap = oit->second;
      std::map<Label, unsigned int>* tsop = &(tsOverlaps[truth]);
      std::map<Label, unsigned int>::iterator it = tsop->find(seg);
      if (it != tsop->end()) it->second += overlap;
      else (*tsop)[seg] = overlap;
    }
  }
  std::map<Label, unsigned int> truthSizeMap;
  getSizeMap<LabelImage>(truthSizeMap, truthImage, false);
  std::map<Label, std::pair<Label, double> > matched;
  for (std::map<Label, std::map<Label, unsigned int> >::const_iterator 
	 tsoit = tsOverlaps.begin(); tsoit != tsOverlaps.end(); ++tsoit) {
    // Find matching with max overlap ratio [O / (A0 + A1 - O)]
    Label truth = tsoit->first;
    Label mseg = BGVAL;
    double mr = -1.0;
    for (std::map<Label, unsigned int>::const_iterator oit = 
	   tsoit->second.begin(); oit != tsoit->second.end(); ++oit) {
      Label seg = oit->first;
      double r = (double)oit->second / 
	(double)(truthSizeMap[truth] + rmap[seg].size() - oit->second);
      if (r > mr) {
	mseg = seg;
	mr = r;
      }
    }
    // Store seg label
    if (mr > overlapRatioThreshold) 
      matched[mseg] = std::make_pair(truth, mr);
  }
  std::list<int> labels;
  for (fTree::const_iterator tit = tree.begin(); tit != tree.end(); 
       ++tit) labels.push_back(matched.count(tit->label) > 0? 1: -1);
  write(labelFileName, labels, 1);
  // // For debug
  // for (std::map<Label, std::pair<Label, double> >::const_iterator 
  // 	 it = matched.begin(); it != matched.end(); ++it) 
  //   std::cout << it->first << " " << it->second.first << " " 
  // 	      << it->second.second << std::endl;
  // // ~ For debug
}



int main (int argc, char* argv[])
{
  if (argc != 5 && argc != 6) {
    std::cerr << "Usage: " << argv[0] 
	      << " initialSegmentationImageName" 
	      << " treeFileName" 
	      << " dilatedTruthImageName" 
	      << " [overlapRatioThreshold = 0.85]"
	      << " labelFileName" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  const char* truthImageName = argv[argi++];
  double overlapRatioThreshold = 
    (argi < argc - 1? atof(argv[argi++]): 0.85);
  const char* labelFileName = argv[argi++];
  operation(labelFileName, initSegImageName, treeFileName, 
	    truthImageName, overlapRatioThreshold);
  return EXIT_SUCCESS;
}
