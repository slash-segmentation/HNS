#include "n3_io.h"
#include "tree2d_util.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* pbImageName, const char* labelImageName, 
		const char* pbFileName, bool invert)
{
  LabelImage::Pointer labelImage = readImage<LabelImage>(labelImageName);
  flist pb;
  read(pb, pbFileName, 1);
  if (invert) 
    for (flist::iterator it = pb.begin(); it != pb.end(); ++it) 
      *it = 1.0 - *it;
  Points bp;
  getPoints(bp, labelImage, BGVAL);
  PointLabelMap lmap;
  getPointNeighbors(lmap, bp, labelImage, CRCONN);
  BoundaryTable bt;
  getBoundaryTable(bt, lmap);  
  if (pb.size() != bt.size()) 
    perr("Error: pb and boundary table size disagree...");
  std::map<Point, int> counts;
  FloatImage::Pointer pbImage = 
    createImage<LabelImage, FloatImage>(labelImage, 0.0);  
  flist::const_iterator pbit = pb.begin();
  for (BoundaryTable::const_iterator bit = bt.begin(); bit != bt.end(); 
       ++bit) {
    for (Points::const_iterator pit = bit->second.begin(); 
	 pit != bit->second.end(); ++pit) {
      std::map<Point, int>::iterator cit = counts.find(*pit);
      double thispb = *pbit;
      if (fabs(thispb) < 1e-4) thispb += 1e-4;
      if (cit != counts.end()) {	
	double v = ((double)getv<FloatImage>(pbImage, pit->x, pit->y) * 
		    (double)cit->second + thispb) / (cit->second + 1);
	setv<FloatImage>(pbImage, pit->x, pit->y, v);
	++(cit->second);
      }
      else {
	setv<FloatImage>(pbImage, pit->x, pit->y, thispb);
	counts[*pit] = 1;
      }
    }
    ++pbit;
  }
  writeImage<FloatImage>(pbImageName, pbImage);
}


int main (int argc, char* argv[])
{
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
	      << " labelImageName"
	      << " pbFileName"
	      << " invertPb"
	      << " pbImageName"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* labelImageName = argv[argi++];
  const char* pbFileName = argv[argi++];
  bool invert = atoi(argv[argi++]) == 0? false: true;
  const char* pbImageName = argv[argi++];
  operation(pbImageName, labelImageName, pbFileName, invert);
  return EXIT_SUCCESS;
}
