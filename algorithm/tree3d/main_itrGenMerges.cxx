#include "tree2d/tree2d_io.h"
#include "tree3d_ml.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

void operation (const char* treeFileName, const char* saliencyFileName, 
		const char* featFileName, const char* labelFileName, 
		const char* initSegImageName, const char* rawImageName, 
		const char* pbImageName, const char* truthImageName, 
		const char* modelFileName)
{
  rf::Model model;
  rf::readModelFromBinaryFile(model, modelFileName);
  LabelImage3::Pointer initSegImage = 
    readImage<LabelImage3>(initSegImageName);
  FloatImage3::Pointer rawImage = readImage<FloatImage3>(rawImageName);
  FloatImage3::Pointer pbImage = readImage<FloatImage3>(pbImageName);
  LabelImage3::Pointer canvas = createImage<LabelImage3>(initSegImage, 0);
  PointMap3 rmap, cmap;
  PointLabelMap3 lmap;
  getPointMap(rmap, lmap, cmap, initSegImage, CRCONN3, false, true);
  PointMap3 bmap;
  getBorderPoints(bmap, initSegImage, false);
  std::list<fMerge> merges;
  if (labelFileName == NULL || truthImageName == NULL) {
    if (featFileName == NULL) 
      getMerges(merges, NULL, NULL, rmap, cmap, lmap, bmap, canvas, 
		rawImage, pbImage, model, NULL);
    else {
      std::list<flist> feats;
      getMerges(merges, &feats, NULL, rmap, cmap, lmap, bmap, canvas, 
		rawImage, pbImage, model, NULL);
      write2(featFileName, feats, false, ' ', '\n');
    }
  }
  else {
    LabelImage3::Pointer truthImage = 
      readImage<LabelImage3>(truthImageName);
    std::list<int> labels;
    if (featFileName == NULL) 
      getMerges(merges, NULL, &labels, rmap, cmap, lmap, bmap, canvas, 
		rawImage, pbImage, model, &truthImage);
    else {
      std::list<flist> feats;
      getMerges(merges, &feats, &labels, rmap, cmap, lmap, bmap, canvas, 
		rawImage, pbImage, model, &truthImage);
      write2(featFileName, feats, false, ' ', '\n');
    }
    write(labelFileName, labels, 1);
  }
  writeMerges(treeFileName, saliencyFileName, merges);
}


int main (int argc, char* argv[])
{
  if (argc != 10) {
    std::cerr << "Usage: " << argv[0]
	      << " initSegImageName"
	      << " rawImageName"
	      << " pbImageName"
	      << " truthImageName ('NULL' to skip)"
	      << " modelFileName"
	      << " treeFileName"
	      << " saliencyFileName"
	      << " featureFileName ('NULL' to skip)"
	      << " labelFileName ('NULL' to skip)"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  const char* rawImageName = argv[argi++];
  const char* pbImageName = argv[argi++];
  const char* truthImageName = 
    strcmp(argv[argi], "NULL") == 0? NULL: argv[argi];
  ++argi;
  const char* modelFileName = argv[argi++];
  const char* treeFileName = argv[argi++];
  const char* saliencyFileName = argv[argi++];
  const char* featFileName = 
    strcmp(argv[argi], "NULL") == 0? NULL: argv[argi];
  ++argi;
  const char* labelFileName = 
    strcmp(argv[argi], "NULL") == 0? NULL: argv[argi];
  ++argi;
  operation(treeFileName, saliencyFileName, featFileName, labelFileName, 
	    initSegImageName, rawImageName, pbImageName, truthImageName, 
	    modelFileName);
  return EXIT_SUCCESS;
}
