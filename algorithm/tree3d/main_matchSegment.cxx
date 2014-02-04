#include "util/metric/metric_util.h"
#include "tree2d/tree2d_io.h"
#include "tree2d/tree_visit.h"
#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

void operation (const char* resImageName, const char* initSegImageName, 
		std::vector<const char*> const& treeFileNames, 
		const char* truthImageName, bool labelCC, bool write16)
{
  LabelImage3::Pointer image = readImage<LabelImage3>(initSegImageName);
  LabelImage3::Pointer truthImage = readImage<LabelImage3>(truthImageName);
  std::map<Label, unsigned int> segSizeMap, truthSizeMap;
  getSizeMap<LabelImage3>(segSizeMap, image, false);
  getSizeMap<LabelImage3>(truthSizeMap, truthImage, false);
  std::map<Label, std::map<Label, unsigned int> > stOverlaps;
  getOverlaps(stOverlaps, image, truthImage, false, false);
  Points3 bg;
  PointMap3 cmap;
  PointLabelMap3 lmap;
  getPoints(bg, image, BGVAL);
  getPointNeighbors(lmap, bg, image, CRCONN3);
  getNeighborPoints(cmap, lmap, true);
  int n = treeFileNames.size();
  std::vector<fTree> trees(n);
  for (int i = 0; i < n; ++i) {
    fTree* pTree = &(trees[i]);
    std::list<fMerge> merges;
    readMerges(merges, treeFileNames[i], NULL);
    getTree(*pTree, merges, &assignDouble);
    PointMap3 cm = cmap;
    PointLabelMap3 lm = lmap;
    std::map<Label, std::map<Label, unsigned int> > sto = stOverlaps;
    std::map<Label, unsigned int> ssm = segSizeMap;
    for (fTree::iterator tit = pTree->begin(); tit != pTree->end(); ++tit) {
      std::map<Label, unsigned int>* psto = &(sto[tit->label]);
      unsigned int* pssm = &(ssm[tit->label]);
      if (tit->child0 >= 0 && tit->child1 >= 0) {
	Label l0 = (*pTree)[tit->child0].label, 
	  l1 = (*pTree)[tit->child1].label;
	add(*psto, sto.find(l0)->second, sto.find(l1)->second);
	Points3 c2rp;
	merge(&c2rp, cm, lm, l0, l1, tit->label, true, false);
	*pssm = ssm.find(l0)->second + ssm.find(l1)->second + c2rp.size();
      }
      std::list<double> stoRatios;
      for (std::map<Label, unsigned int>::const_iterator stoit = 
	     psto->begin(); stoit != psto->end(); ++stoit)
	stoRatios.push_back(getr(stoit->second, *pssm + 
				 truthSizeMap.find(stoit->first)->second 
				 - stoit->second));
      tit->data = getMax(stoRatios, false);
      // // For debug
      // std::cerr << tit->label << ": " << tit->data << std::endl;
      // // ~ For debug
    }
  }
  // // For debug
  // std::cerr << "start resolving" << std::endl;
  // // ~ For debug
  std::list<IntPair> picks;
  resolve(picks, trees, &compDouble, &assignDouble);
  // Paint
  std::map<Label, Label> init2final;
  Label labelToAssign = 1;
  for (std::list<IntPair>::const_iterator pit = picks.begin(); 
       pit != picks.end(); ++pit) {
    std::list<int> leaves;
    trees[pit->first].get_leaves(leaves, pit->second);
    for (std::list<int>::const_iterator lit = leaves.begin(); 
	 lit != leaves.end(); ++lit) 
      init2final[trees[pit->first][*lit].label] = labelToAssign;
    ++labelToAssign;
  }
  // Keep previous initial/final label to avoid some lookups
  Label prevInit = 0, prevFinal = 0;
  for (itk::ImageRegionIterator<LabelImage3>
	 iit(image, image->GetLargestPossibleRegion()); !iit.IsAtEnd(); 
       ++iit) {
    Label init = iit.Get();
    if (init != prevInit) {
      prevInit = init;
      prevFinal = init2final.find(init)->second;
    }
    iit.Set(prevFinal);
  }
  // Remove exclusive gaps
  removeExclusiveGaps<LabelImage3>(image, BGVAL, CRCONN3);
  // Output 
  if (write16) {
    UInt16Image3::Pointer outImage = 
      (labelCC? 
       labelConnectedComponents<LabelImage3, UInt16Image3>(image, BGVAL): 
       castImage<LabelImage3, UInt16Image3>(image));
    writeImage<UInt16Image3>(resImageName, outImage);
  }
  else {
    if (labelCC) 
      image = 
	labelConnectedComponents<LabelImage3, LabelImage3>(image, BGVAL);
    writeImage<LabelImage3>(resImageName, image);
  }
}



int main (int argc, char* argv[])
{
  if (argc < 6) {
    std::cerr << "Usage: " << argv[0] 
	      << " initSegImageName"
	      << " {-t treeFileName}"
	      << " truthImageName"
	      << " [labelOutputConnectedComponents = 1]"
	      << " [writeToUInt16Image = 0]" 
	      << " bestMatchSegImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* initSegImageName = argv[argi++];
  std::vector<const char*> treeFileNames;
  treeFileNames.reserve((argc - 4) / 2);
  while (strcmp(argv[argi], "-t") == 0) {
    treeFileNames.push_back(argv[argi + 1]);
    argi += 2;
  }
  const char* truthImageName = argv[argi++];
  bool labelCC = argi < argc - 1 && atoi(argv[argi++]) == 0? false: true;
  bool write16 = argi < argc - 1 && atoi(argv[argi++]) == 1? true: false;
  const char* resImageName = argv[argi++];
  operation(resImageName, initSegImageName, treeFileNames, 
	    truthImageName, labelCC, write16);
  return EXIT_SUCCESS;
}
