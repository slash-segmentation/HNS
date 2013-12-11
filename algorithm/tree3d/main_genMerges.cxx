#include "tree3d_util.h"
#include "tree2d/tree2d_io.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

void operation (const char* treeFileName, const char* saliencyFileName, 
		const char* labelImageName, const char* valImageName)
{
  LabelImage3::Pointer labelImage = readImage<LabelImage3>(labelImageName);
  FloatImage3::Pointer valImage = readImage<FloatImage3>(valImageName);
  std::list<fMerge> merges;
  getMerges(merges, labelImage, valImage);
  writeMerges(treeFileName, saliencyFileName, merges);
}



int main (int argc, char* argv[])
{
  if (argc != 4 && argc != 5) {
    std::cerr << "Usage: " << argv[0]
	      << " labelImageName"
	      << " boundaryValueImageName"
	      << " treeFileName"
	      << " saliencyFileName ('NULL to skip')"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* valImageName = argv[argi++];
  const char* treeFileName = argv[argi++];
  const char* saliencyFileName = 
    strcmp(argv[argi++], "NULL") == 0? NULL: argv[argi - 1]; 
  operation(treeFileName, saliencyFileName, labelImageName, valImageName);
  return EXIT_SUCCESS;
}
