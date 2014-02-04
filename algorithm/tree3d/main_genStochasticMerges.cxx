#include "tree2d/tree2d_io.h"
#include "perturb.h"
#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

void operation (const char* treeFileName, const char* saliencyFileName, 
		const char* labelImageName, const char* valImageName, 
		int metric, long const* seed)
{
  LabelImage3::Pointer labelImage = readImage<LabelImage3>(labelImageName);
  FloatImage3::Pointer valImage = readImage<FloatImage3>(valImageName);
  Points3 bp;
  getPoints(bp, labelImage, BGVAL);
  std::list<fMerge> merges;
  if (seed == NULL) 
    getMerges(merges, bp, labelImage, valImage); // No randomness
  else getMerges(merges, bp, labelImage, valImage, metric, *seed);
  writeMerges(treeFileName, saliencyFileName, merges);
}


int main (int argc, char* argv[])
{
  if (argc != 7) {
    std::cerr << "Usage: " << argv[0]
	      << " labelImageName" 
	      << " boundaryValImageName"
	      << " boundaryMetric (1: min | 2: mean | 3: median)"
	      << " seed ('NULL': no randomness | 'CT': current time | #)"
	      << " treeFileName"
	      << " saliencyFileName ('NULL' to skip)"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* valImageName = argv[argi++];
  int metric = atoi(argv[argi++]);
  long* seed = NULL;
  long tseed;
  if (strcmp(argv[argi], "NULL") != 0) {
    if (strcmp(argv[argi], "CT") == 0) tseed = std::time(NULL);
    else tseed = atol(argv[argi]);
    seed = &tseed;
  }
  ++argi;
  const char* treeFileName = argv[argi++];
  const char* saliencyFileName = 
    strcmp(argv[argi], "NULL") == 0? NULL: argv[argi];
  ++argi;
  operation(treeFileName, saliencyFileName, labelImageName, valImageName, 
	    metric, seed);
  return EXIT_SUCCESS;
}
