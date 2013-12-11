#include "perturb.h"
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/discrete_distribution.hpp"
using namespace n3;
using namespace n3::tree2d;

// bp: all 0-pixel points
void n3::tree2d::getMerges (std::list<fMerge>& merges,
			    LabelImage::Pointer labelImage,
			    FloatImage::Pointer valImage, 
			    int metric, const long seed)
{
  boost::mt19937 rng(seed);
  Points bp; 
  getPoints(bp, labelImage, BGVAL);
  PointLabelMap lmap;
  getPointNeighbors(lmap, bp, labelImage, CRCONN);
  BoundaryTable bt;
  getBoundaryTable(bt, lmap);
  Label labelToAssign = getMax<LabelImage>(labelImage) + 1;
  while (bt.size() > 0) {
    int n = bt.size();
    std::vector<double> probs;
    std::vector<LabelPair> pairs;
    probs.reserve(n);
    pairs.reserve(n);
    for (BoundaryTable::const_iterator bit = bt.begin(); bit != bt.end(); 
	 ++bit) {
      std::list<Float> vals;
      getvs<FloatImage>(vals, valImage, bit->second);
      Float v;
      if (metric == 1) v = getMin(vals);
      else if (metric == 2) v = getMean(vals);
      else if (metric == 3) v = getMedian(vals);
      else perr("Error: unsupported boundary metric...");
      probs.push_back(1.0 - v);
      pairs.push_back(bit->first);
    }
    boost::random::discrete_distribution<> dist(probs);
    int pick = dist(rng);
    Label r0 = pairs[pick].first, r1 = pairs[pick].second;
    merges.push_back(fMerge(r0, r1, labelToAssign, 1.0 - probs[pick]));
    updateBoundaryTable(bt, r0, r1, labelToAssign);
    ++labelToAssign;
  }
}
