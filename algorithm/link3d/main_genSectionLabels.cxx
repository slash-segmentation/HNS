#include "util/metric/metric_util.h"
#include "link3d_io.h"
using namespace n3;
using namespace n3::link3d;

void getSegToSegLabel 
(std::list<int>& labels, std::list<fLink> const& segPairs, 
 std::set<LabelPair> const& truthPairs, 
 std::map<Label, std::pair<Label, unsigned int> > const& segToTruth0, 
 std::map<Label, std::pair<Label, unsigned int> > const& segToTruth1)
{
  for (std::list<fLink>::const_iterator pit = segPairs.begin(); 
       pit != segPairs.end(); ++pit) {
    Label seg0 = pit->r0, seg1 = pit->r1;
    std::map<Label, std::pair<Label, unsigned int> >::const_iterator 
      stit0 = segToTruth0.find(seg0), stit1 = segToTruth1.find(seg1);
    int label = -1;
    if (stit0 != segToTruth0.end() && stit1 != segToTruth1.end() && 
	truthPairs.count(std::make_pair(stit0->second.first, 
					stit1->second.first)) > 0)
      label = 1;
    labels.push_back(label);
  }
}


void operation (const char* labelFileName, const char* segPairFileName, 
		const char* truthPairFileName, const char* segImageName0, 
		const char* segImageName1, const char* truthImageName0, 
		const char* truthImageName1)
{
  std::list<fLink> segPairs;
  readLinks(segPairs, segPairFileName, NULL);
  std::set<LabelPair> truthPairs;
  readPairs(truthPairs, truthPairFileName, ' ', '\n');
  LabelImage::Pointer segImage0 = readImage<LabelImage>(segImageName0);
  LabelImage::Pointer segImage1 = readImage<LabelImage>(segImageName1);
  LabelImage::Pointer truthImage0 = readImage<LabelImage>(truthImageName0);
  LabelImage::Pointer truthImage1 = readImage<LabelImage>(truthImageName1);
  std::map<Label, std::pair<Label, unsigned int> > segToTruth0;
  std::map<Label, std::pair<Label, unsigned int> > segToTruth1;
  match(segToTruth0, segImage0, truthImage0, 0);
  match(segToTruth1, segImage1, truthImage1, 0);
  std::list<int> labels;
  getSegToSegLabel(labels, segPairs, truthPairs, segToTruth0, segToTruth1);
  write(labelFileName, labels, 1);
}


int main (int argc, char* argv[])
{
  if (argc != 8) {
    std::cerr << "Usage: " << argv[0] 
	      << " regionPairFileName" 
	      << " truthPairFileName" 
	      << " labelImageName0"
	      << " labelImageName1" 
	      << " truthImageName0" 
	      << " truthImageName1"
	      << " labelFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* segPairFileName = argv[argi++];
  const char* truthPairFileName = argv[argi++];
  const char* segImageName0 = argv[argi++];
  const char* segImageName1 = argv[argi++];
  const char* truthImageName0 = argv[argi++];
  const char* truthImageName1 = argv[argi++];
  const char* labelFileName = argv[argi++];
  operation(labelFileName, segPairFileName, truthPairFileName, 
	    segImageName0, segImageName1, truthImageName0, truthImageName1);
  return EXIT_SUCCESS;
}
