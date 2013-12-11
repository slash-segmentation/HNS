#include "util/metric/metric_util.h"
#include "link3d_io.h"
using namespace n3;
using namespace n3::link3d;

// void getBodyToBodyLabel 
// (std::list<int>& labels, std::list<fBodyLink> const& bodyPairs, 
//  boost::unordered_map<Label, fBody const*> bodies, 
//  std::map<IntPair, boost::unordered_set<LabelPair> > const& truthPairs, 
//  std::map<int, std::map<Label, std::pair<Label, unsigned int> > >& 
//  segToTruth, 
//  std::map<int, LabelImage::Pointer>& segImages, 
//  std::map<int, LabelImage::Pointer>& truthImages, 
//  std::map<int, const char*> const& segImageNames, 
//  std::map<int, const char*> const& truthImageNames)
void getBodyToBodyLabel 
(std::list<int>& labels, std::list<fBodyLink> const& bodyPairs, 
 std::map<Label, fBody const*> bodies, 
 std::map<IntPair, std::set<LabelPair> > const& truthPairs, 
 std::map<int, std::map<Label, std::pair<Label, unsigned int> > >& 
 segToTruth, 
 std::map<int, LabelImage::Pointer>& segImages, 
 std::map<int, LabelImage::Pointer>& truthImages, 
 std::map<int, const char*> const& segImageNames, 
 std::map<int, const char*> const& truthImageNames)
{
  for (std::list<fBodyLink>::const_iterator pit = bodyPairs.begin(); 
       pit != bodyPairs.end(); ++pit) {
    fBody const* b0 = bodies[pit->nodes[0].first];
    fBody const* b1  = bodies[pit->nodes[1].first];
    BodyNodeData const* d0 = 
      &(b0->nodes.find(pit->nodes[0].second)->second.data);
    BodyNodeData const* d1 = 
      &(b1->nodes.find(pit->nodes[1].second)->second.data);
    int slice0 = d0->slice, slice1 = d1->slice;
    Label seg0 = d0->label2, seg1 = d1->label2;
    std::map<int, std::map<Label, std::pair<Label, unsigned int> > >::
      iterator sit0 = segToTruth.find(slice0);
    std::map<Label, std::pair<Label, unsigned int> >* ps0;
    if (sit0 == segToTruth.end()) {
      LabelImage::Pointer segImage0 = 
	getImage<LabelImage>(segImages, segImageNames, slice0);
      LabelImage::Pointer truthImage0 = 
	getImage<LabelImage>(truthImages, truthImageNames, slice0);
      ps0 = &(segToTruth[slice0]);
      match(*ps0, segImage0, truthImage0, 0);
    }
    else ps0 = &(sit0->second);
    std::map<int, std::map<Label, std::pair<Label, unsigned int> > >::
      iterator sit1 = segToTruth.find(slice1);
    std::map<Label, std::pair<Label, unsigned int> >* ps1;
    if (sit1 == segToTruth.end()) {
      LabelImage::Pointer segImage1 = 
	getImage<LabelImage>(segImages, segImageNames, slice1);
      LabelImage::Pointer truthImage1 = 
	getImage<LabelImage>(truthImages, truthImageNames, slice1);
      ps1 = &(segToTruth[slice1]);
      match(*ps1, segImage1, truthImage1, 0);
    }
    else ps1 = &(sit1->second);
    std::map<Label, std::pair<Label, unsigned int> >::const_iterator 
      stit0 = ps0->find(seg0), stit1 = ps1->find(seg1);
    int label = -1;
    if (stit0 != ps0->end() && stit1 != ps1->end()) {
      IntPair key = slice0 < slice1? std::make_pair(slice0, slice1): 
	std::make_pair(slice1, slice0);
      // std::map<IntPair, 
      // 	       boost::unordered_set<LabelPair> >::const_iterator 
      // 			     tpit = truthPairs.find(key);
      std::map<IntPair, std::set<LabelPair> >::const_iterator 
			     tpit = truthPairs.find(key);
      if (tpit->second.count(std::make_pair(stit0->second.first, 
					    stit1->second.first)) > 0)
      label = 1;
    }
    labels.push_back(label);
  }
}



void operation (const char* labelFileName, const char* bodyFileName, 
		const char* bodyPairFileName, 
		std::map<int, const char*> const& segImageNames, 
		std::map<int, const char*> const& truthImageNames, 
		std::map<IntPair, const char*> truthPairFileNames)
{
  std::list<fBody> bodies;
  readBodies(bodies, bodyFileName, NULL);
  // boost::unordered_map<Label, fBody const*> pbs;
  std::map<Label, fBody const*> pbs;
  for (std::list<fBody>::const_iterator bit = bodies.begin(); 
       bit != bodies.end(); ++bit) pbs[bit->label] = &(*bit);
  std::list<fBodyLink> bodyPairs;
  readBodyLinks(bodyPairs, bodyPairFileName, NULL);
  std::map<int, LabelImage::Pointer> segImages, truthImages;
  std::map<int, std::map<Label, std::pair<Label, unsigned int> > > 
    segToTruth;
  // std::map<IntPair, boost::unordered_set<LabelPair> > truthPairs;
  std::map<IntPair, std::set<LabelPair> > truthPairs;
  for (std::map<IntPair, const char*>::const_iterator nit = 
	 truthPairFileNames.begin(); nit != truthPairFileNames.end(); 
       ++nit) readPairs(truthPairs[nit->first], nit->second, ' ', '\n');
  std::list<int> labels;
  getBodyToBodyLabel(labels, bodyPairs, pbs, truthPairs, segToTruth, 
		     segImages, truthImages, segImageNames, 
		     truthImageNames);
  write(labelFileName, labels, 1);
}



int main (int argc, char* argv[])
{
  if (argc < 12) {
    std::cerr << "Usage: " << argv[0] 
	      << " bodyFileName"
	      << " bodyPairFileName"
	      << " {-i sliceNo 2dSegImageName 2dTruthImageName}"
	      << " {-p sliceNo0 sliceNo1 (> sliceNo0) truthPairFileName}"
	      << " labelFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* bodyFileName = argv[argi++];
  const char* bodyPairFileName = argv[argi++];
  std::map<int, const char*> segImageNames, truthImageNames;
  while (strcmp(argv[argi], "-i") == 0) {
    ++argi;
    int slice = atoi(argv[argi++]);
    segImageNames[slice] = argv[argi++];
    truthImageNames[slice] = argv[argi++];
  }
  std::map<IntPair, const char*> truthPairFileNames;
  while (strcmp(argv[argi], "-p") == 0) {
    ++argi;
    int slice0 = atoi(argv[argi++]);
    int slice1 = atoi(argv[argi++]);
    if (slice0 >= slice1) 
      perr("Error: sliceNo0 needs to be smaller than sliceNo1...");
    truthPairFileNames[std::make_pair(slice0, slice1)] = argv[argi++];
  }
  const char* labelFileName = argv[argi++];
  operation(labelFileName, bodyFileName, bodyPairFileName, segImageNames,
	    truthImageNames, truthPairFileNames);
  return EXIT_SUCCESS;
}
