#include "perturb.h"
using namespace n3;
using namespace n3::tree3d;

// Metric: 1: min, 2: mean, 3: median
void n3::tree3d::getMerges (std::list<tree2d::fMerge>& merges, 
			    Points3 const& bp, 
			    LabelImage3::Pointer labelImage, 
			    FloatImage3::Pointer valImage, 
			    int metric, const long seed)
{
  boost::mt19937 rng(seed);
  PointLabelMap3 lmap;
  getPointNeighbors(lmap, bp, labelImage, CRCONN3);
  Label r01 = getMaxLabel(lmap) + 1;
  BoundaryTable3 bt;
  getBoundaryTable(bt, lmap, valImage);
  while (!bt.empty()) {
    int n = bt.size();
    std::vector<Float> probs;
    std::vector<LabelPair> pairs;
    probs.reserve(n);
    pairs.reserve(n);
    for (BoundaryTable3::const_iterator bit = bt.begin(); bit != bt.end(); 
	 ++bit) {
      std::list<Float> vals;
      for (std::list<fPixel3>::const_iterator pit = bit->second.begin(); 
	   pit != bit->second.end(); ++pit) vals.push_back(pit->val);
      Float v;
      if (metric == 1) v = getMin(vals, true);
      else if (metric == 2) v = getMean(vals);
      else if (metric == 3) v = getMedian(vals, true);
      else perr("Error: unsupported boundary metric...");
      probs.push_back(1.0 - v);
      pairs.push_back(bit->first);
    }
    boost::random::discrete_distribution<> dist(probs);
    int pick = dist(rng);
    Label r0 = pairs[pick].first, r1 = pairs[pick].second;
    updateBoundaryTable(bt, NULL, NULL, r0, r1, r01);
    merges.push_back(tree2d::fMerge(r0, r1, r01, 1.0 - probs[pick]));
    merge(NULL, lmap, r0, r1, r01);
    ++r01;
  }
}
