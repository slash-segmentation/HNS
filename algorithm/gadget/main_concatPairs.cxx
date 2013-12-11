#include "util/io_util.h"
using namespace n3;

void operation (const char* outputPairFileName, 
		const char* inputPairFileName01, 
		const char* inputPairFileName12)
{
  std::list<LabelPair> pair01, pair12;
  readPairs(pair01, inputPairFileName01, ' ', '\n');
  readPairs(pair12, inputPairFileName12, ' ', '\n');
  std::map<Label, std::set<Label> > pmap01, pmap12;
  for (std::list<LabelPair>::const_iterator pit = pair01.begin(); 
       pit != pair01.end(); ++pit) 
    pmap01[pit->first].insert(pit->second);
  for (std::list<LabelPair>::const_iterator pit = pair12.begin(); 
       pit != pair12.end(); ++pit)
    pmap12[pit->first].insert(pit->second);
  std::list<LabelPair> pair02;
  for (std::map<Label, std::set<Label> >::const_iterator pit01 = 
	 pmap01.begin(); pit01 != pmap01.end(); ++pit01) {
    for (std::set<Label>::const_iterator sit01 = pit01->second.begin(); 
	 sit01 != pit01->second.end(); ++sit01) {
      std::map<Label, std::set<Label> >::const_iterator pit12 = 
	pmap12.find(*sit01);
      if (pit12 != pmap12.end()) 
	for (std::set<Label>::const_iterator sit02 = pit12->second.begin(); 
	     sit02 != pit12->second.end(); ++sit02) 
	  pair02.push_back(std::make_pair(pit01->first, *sit02));
    }    
  }
  pair02.sort();
  pair02.unique();
  writePairs(outputPairFileName, pair02, ' ', '\n');
}


int main (int argc, char* argv[])
{
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] 
	      << " inputPairFileName01"
	      << " inputPairFileName12"
	      << " outputPairFileName02"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* inputPairFileName01 = argv[argi++];
  const char* inputPairFileName12 = argv[argi++];
  const char* outputPairFileName02 = argv[argi++];
  operation(outputPairFileName02, inputPairFileName01, 
	    inputPairFileName12);
  return EXIT_SUCCESS;
}
