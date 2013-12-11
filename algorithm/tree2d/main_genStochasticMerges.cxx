#include <ctime>
#include "perturb.h"
#include "tree2d_util.h"
#include "tree2d_io.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* treeFileName, const char* saliencyFileName, 
		const char* labelImageName, const char* valImageName, 
		int metric, long const* seed)
{
  LabelImage::Pointer labelImage = readImage<LabelImage>(labelImageName);
  FloatImage::Pointer valImage = readImage<FloatImage>(valImageName);
  std::list<fMerge> merges;
  if (seed != NULL) getMerges(merges, labelImage, valImage, metric, *seed);
  else getMerges(merges, labelImage, valImage, metric);
  writeMerges(treeFileName, saliencyFileName, merges);
}



int main (int argc, char* argv[])
{
  if (argc != 6 && argc != 7) {
    std::cerr << "Usage: " << argv[0] 
	      << " labelImageName" 
	      << " boundaryValueImageName" 
	      << " [boundaryMetric = 3 (1: min | 2: mean | 3: median)]"
	      << " randomSeed ('NULL': no random | 'CT': use current time)"
	      << " treeFileName" 
	      << " saliencyFileName ('NULL' to skip)"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* valImageName = argv[argi++];
  int metric = argi < argc - 3? atoi(argv[argi++]): 3;
  long* seed = NULL;
  long tseed;
  if (strcmp(argv[argi], "NULL") != 0) {    
    if (strcmp(argv[argi], "CT")) tseed = std::time(NULL);
    else tseed = atol(argv[argi]);
    seed = &tseed;
  }
  ++argi;
  const char* treeFileName = argv[argi++];
  const char* saliencyFileName = 
    strcmp(argv[argi], "NULL") == 0? NULL: argv[argi];
  operation(treeFileName, saliencyFileName, labelImageName, valImageName, 
	    metric, seed);
  return EXIT_SUCCESS;
}
