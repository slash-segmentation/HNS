#include "n3_io.h"
#include "n3_util.h"
using namespace n3;

struct Data {
  int label;
  flist feat;
  Data (flist* pf, int label = 0) : label(label) {splice(feat, *pf, false);}
  bool operator < (Data const& d) {return compContainer(feat, d.feat);}
  bool operator == (Data const& d) {
    return is_equal(feat, d.feat) && label == d.label;
  }
};



void operation (const char* uniqueFeatFileName, 
		const char* uniqueLabelFileName, 
		std::vector<const char*> const& featFileNames, 
		std::vector<const char*> const& labelFileNames)
{
  int n = featFileNames.size();
  std::list<Data> data;
  for (int i = 0; i < n; ++i) {
    std::list<flist> f;
    read2(f, featFileNames[i]);
    std::list<Data> d;
    std::list<flist>::iterator fit = f.begin(); 
    while (fit != f.end()) {
      d.push_back(Data(&(*fit)));
      ++fit;
    }
    if (uniqueLabelFileName != NULL) {
      std::list<int> l;
      read(l, labelFileNames[i], 1, ' ');
      std::list<int>::const_iterator lit = l.begin();
      std::list<Data>::iterator dit = d.begin(); 
      while (dit != d.end()) {
	dit->label = *lit;
	++lit;
	++dit;
      }
    }
    d.sort();
    merge(data, d, true);
  }
  std::list<flist> f;
  for (std::list<Data>::iterator dit = data.begin(); dit != data.end(); 
       ++dit) {
    f.push_back(flist());
    splice(f.back(), dit->feat, false);
  }
  write2(uniqueFeatFileName, f, false, ' ', '\n');
  if (uniqueLabelFileName != NULL) {
    std::list<int> l;
    for (std::list<Data>::const_iterator dit = data.begin(); 
	 dit != data.end(); ++dit) l.push_back(dit->label);
    write(uniqueLabelFileName, l, 1, ' ');
  }
}



int main (int argc, char* argv[])
{
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0]
	      << " {-f featureFileName}"
	      << " [{-l labelFileName}]"
	      << " uniqueFeatureFileName"
	      << " uniqueLabelFileName ('NULL' to skip)"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  std::vector<const char*> featFileNames, labelFileNames;
  featFileNames.reserve((argc - 3) / 2);
  labelFileNames.reserve((argc - 3) / 2);
  while (argi < argc - 2) {
    if (strcmp(argv[argi], "-f") == 0) 
      featFileNames.push_back(argv[argi + 1]);
    else if (strcmp(argv[argi], "-l") == 0)
      labelFileNames.push_back(argv[argi + 1]);
    argi += 2;
  }
  if (!labelFileNames.empty() && 
      labelFileNames.size() != featFileNames.size()) 
    perr("Error: feature and label file number disagree...");
  const char* uniqueFeatFileName = argv[argi++];
  const char* uniqueLabelFileName = argv[argi++];
  operation(uniqueFeatFileName, uniqueLabelFileName, featFileNames, 
	    labelFileNames);
  return EXIT_SUCCESS;
}
