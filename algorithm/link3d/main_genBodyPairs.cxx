#include "util/struct_util.h"
#include "link3d_io.h"
using namespace n3;
using namespace n3::link3d;

void operation (const char* bodyPairFileName, const char* bodyFileName, 
		std::map<int, const char*> const& labelImageNames2, 
		std::map<int, const char*> const& labelImageNames3)
{
  std::list<fBody> bodies;
  readBodies(bodies, bodyFileName, NULL);
  std::set<int> slices;
  for (std::map<int, const char*>::const_reverse_iterator it = 
	 labelImageNames2.rbegin(); it != labelImageNames2.rend(); ++it) 
    slices.insert(it->first);
  int bottomSlice = *slices.begin(), topSlice = *slices.rbegin();
  std::map<Label, std::list<std::pair<int, int> > > terminals;
  std::map<Label, fBody*> pbs;
  for (std::list<fBody>::iterator bit = bodies.begin(); 
       bit != bodies.end(); ++bit) {
    pbs[bit->label] = &(*bit);
    bit->get_terminals(terminals[bit->label], false, 
		       bottomSlice, topSlice);
  }
  std::map<int, LabelImage::Pointer> labelImages2, labelImages3;
  std::map<int, PointMap> rmaps;
  std::list<fBodyLink> links;
  // For every body
  for (std::map<Label, std::list<std::pair<int, int> > >::const_iterator 
	 tit0 = terminals.begin(); tit0 != terminals.end(); ++tit0) {
    Label l3 = tit0->first;
    fBody const* pb = pbs[l3];
    // For every terminal
    for (std::list<std::pair<int, int> >::const_iterator tit1 = 
	   tit0->second.begin(); tit1 != tit0->second.end(); ++tit1) {
      int nid = tit1->first;
      int slice = (pb->nodes.find(nid)->second).data.slice;
      Label l2 = (pb->nodes.find(nid)->second).data.label2;
      if (tit1->second == 1 || tit1->second == 3) {
	PointMap* prm = getPointMap(rmaps, labelImages2, 
				    labelImageNames2, slice);
	Points* pr = &(*prm)[l2];
	int prevSlice = prev(slices, slice);
	LabelImage::Pointer image2p = 
	  getImage<LabelImage>(labelImages2, labelImageNames2, prevSlice);
	LabelImage::Pointer image3p = 
	  getImage<LabelImage>(labelImages3, labelImageNames3, prevSlice);
	std::set<std::pair<Label, Label> > lps; // (l2p, l3p)
	for (Points::const_iterator pit = pr->begin(); 
	     pit != pr->end(); ++pit) {
	  Label l2p = getv<LabelImage>(image2p, pit->x, pit->y);
	  if (l2p != BGVAL) {
	    Label l3p = getv<LabelImage>(image3p, pit->x, pit->y);
	    lps.insert(std::make_pair(l2p, l3p));
	  }
	}
	for (std::set<std::pair<Label, Label> >::const_iterator lit = 
	       lps.begin(); lit != lps.end(); ++lit) {
	  int nidp = pbs[lit->second]->slmap[prevSlice][lit->first];
	  links.push_back(fBodyLink(lit->second, nidp, l3, nid));
	}
      }
      if (tit1->second == 2 || tit1->second == 3) {
	PointMap* prm = getPointMap(rmaps, labelImages2, 
				    labelImageNames2, slice);
	Points* pr = &(*prm)[l2];
	int nextSlice = next(slices, slice);
	LabelImage::Pointer image2n = 
	  getImage<LabelImage>(labelImages2, labelImageNames2, nextSlice);
	LabelImage::Pointer image3n = 
	  getImage<LabelImage>(labelImages3, labelImageNames3, nextSlice);
	std::set<std::pair<Label, Label> > lps;
	for (Points::const_iterator pit = pr->begin(); 
	     pit != pr->end(); ++pit) {
	  Label l2n = getv<LabelImage>(image2n, pit->x, pit->y);
	  if (l2n != BGVAL) {
	    Label l3n = getv<LabelImage>(image3n, pit->x, pit->y);
	    lps.insert(std::make_pair(l2n, l3n));
	  }
	}
	for (std::set<std::pair<Label, Label> >::const_iterator lit = 
	       lps.begin(); lit != lps.end(); ++lit) {
	  int nidp = pbs[lit->second]->slmap[nextSlice][lit->first];
	  links.push_back(fBodyLink(l3, nid, lit->second, nidp));
	}
      }
    }
  }
  writeBodyLinks(bodyPairFileName, NULL, links);
}



int main (int argc, char* argv[])
{
  if (argc < 7) {
    std::cerr << "Usage: " << argv[0] 
	      << " bodyFileName"
	      << " {-i sliceNo 2dSegImageName 3dSegImageName}"
	      << " bodyPairFileName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* bodyFileName = argv[argi++];
  std::map<int, const char*> labelImageNames2, labelImageNames3;
  while (argi < argc - 1) {
    if (strcmp(argv[argi++], "-i") != 0) 
      perr("Error: incorrect input argument tag...");
    int slice = atoi(argv[argi++]);
    labelImageNames2[slice] = argv[argi++];
    labelImageNames3[slice] = argv[argi++];
  }
  const char* bodyPairFileName = argv[argi++];
  operation(bodyPairFileName, bodyFileName, 
	    labelImageNames2, labelImageNames3);
  return EXIT_SUCCESS;
}
