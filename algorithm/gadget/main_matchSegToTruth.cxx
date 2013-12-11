#include "util/metric/metric_util.h"
using namespace n3;

void operation (std::vector<const char*> const& segImageNames, 
		std::vector<const char*> const& truthImageNames)
{
  int n = segImageNames.size();
  std::vector<LabelImage::Pointer> segImages(n), truthImages(n);
  for (int i = 0; i < n; ++i) {
    segImages[i] = readImage<LabelImage>(segImageNames[i]);
    truthImages[i] = readImage<LabelImage>(truthImageNames[i]);
  }
  std::map<Label, std::map<Label, unsigned int> > overlaps;
  for (int i = 0; i < n; ++i) {
    std::map<Label, std::map<Label, unsigned int> > o;
    getOverlaps(o, segImages[i], truthImages[i], false, false);
    for (std::map<Label, std::map<Label, unsigned int> >::const_iterator 
	   oit00 = o.begin(); oit00 != o.end(); ++oit00) {
      std::map<Label, std::map<Label, unsigned int> >::iterator oit10 = 
	overlaps.find(oit00->first);
      if (oit10 != overlaps.end()) {
	for (std::map<Label, unsigned int>::const_iterator oit01 = 
	       oit00->second.begin(); oit01 != oit00->second.end();
	     ++oit01) {
	  std::map<Label, unsigned int>::iterator oit11 = 
	    oit10->second.find(oit01->first);
	  if (oit11 != oit10->second.end()) 
	    (oit10->second)[oit01->first] += oit01->second;
	  else (oit10->second)[oit01->first] = oit01->second;
	}
      }
      else overlaps[oit00->first] = oit00->second;
    }
  }
  std::map<Label, std::list<Label> > matches; // Truth to segs
  for (std::map<Label, std::map<Label, unsigned int> >::const_iterator 
	 oit = overlaps.begin(); oit != overlaps.end(); ++oit) {
    std::pair<Label, unsigned int> m;
    if (findMax(m, oit->second)) matches[m.first].push_back(oit->first);    
  }
  for (std::map<Label, std::list<Label> >::const_iterator mit = 
	 matches.begin(); mit != matches.end(); ++mit) {
    std::cout << mit->first << ": ";
    for (std::list<Label>::const_iterator sit = mit->second.begin(); 
	 sit != mit->second.end(); ++sit)
      std::cout << *sit << " ";
    std::cout << std::endl;
  }
}



int main (int argc, char* argv[])
{
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0] 
	      << " startSliceNo" 
	      << " sliceNum"
	      << " segImageNameTemplate"
	      << " truthImageNameTemplate"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  int startSliceNo = atoi(argv[argi++]);
  int sliceNum = atoi(argv[argi++]);
  const char* segImageNameTemplate = argv[argi++];
  const char* truthImageNameTemplate = argv[argi++];
  std::vector<const char*> segImageNames, truthImageNames;
  segImageNames.reserve(sliceNum);
  truthImageNames.reserve(sliceNum);
  for (int i = 0; i < sliceNum; i++) {
    char* p = new char[1000];
    sprintf(p, segImageNameTemplate, i + startSliceNo);
    segImageNames.push_back(p);
    p = new char[1000];
    sprintf(p, truthImageNameTemplate, i + startSliceNo);
    truthImageNames.push_back(p);
  }
  operation(segImageNames, truthImageNames);
  for (int i = 0; i < sliceNum; ++i) {
    delete[] segImageNames[i];
    delete[] truthImageNames[i];
  }
  return EXIT_SUCCESS;
}
