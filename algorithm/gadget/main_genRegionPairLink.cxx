#include "util/struct_util.h"
#include "util/io_util.h"
using namespace n3;

void operation (const char* pairFileName, const char* labelImage2Name0, 
		const char* labelImage2Name1, const char* labelImage3Name0, 
		const char* labelImage3Name1)
{
  LabelImage::Pointer 
    labelImage20 = readImage<LabelImage>(labelImage2Name0), 
    labelImage21 = readImage<LabelImage>(labelImage2Name1), 
    labelImage30 = readImage<LabelImage>(labelImage3Name0), 
    labelImage31 = readImage<LabelImage>(labelImage3Name1);
  std::set<LabelPair> pairs;
  itk::ImageRegionConstIterator<LabelImage> 
    iit20(labelImage20, labelImage20->GetLargestPossibleRegion()), 
    iit21(labelImage21, labelImage21->GetLargestPossibleRegion()), 
    iit30(labelImage30, labelImage20->GetLargestPossibleRegion()), 
    iit31(labelImage31, labelImage21->GetLargestPossibleRegion());
  while (!iit20.IsAtEnd()) {
    if (iit20.Get() != BGVAL && iit21.Get() != BGVAL && 
	iit30.Get() != BGVAL && iit31.Get() != BGVAL && 
	iit30.Get() == iit31.Get())
      pairs.insert(std::make_pair(iit20.Get(), iit21.Get()));
    ++iit20;
    ++iit21;
    ++iit30;
    ++iit31;
  }
  writePairs(pairFileName, pairs, ' ', '\n');
}



int main (int argc, char* argv[])
{
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0]
	      << " 2dLabelImageName0"
	      << " 2dLabelImageName1"
	      << " 3dLabelImageName0"
	      << " 3dLabelImageName1"
	      << " regionPairFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImage2Name0 = argv[argi++];
  const char* labelImage2Name1 = argv[argi++];
  const char* labelImage3Name0 = argv[argi++];
  const char* labelImage3Name1 = argv[argi++];
  const char* pairFileName = argv[argi++];
  operation(pairFileName, labelImage2Name0, labelImage2Name1, 
	    labelImage3Name0, labelImage3Name1);
  return EXIT_SUCCESS;
}
