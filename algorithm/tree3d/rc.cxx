#include "rc.h"
using namespace n3;
using namespace n3::tree3d;

// Start from longest edge
// Count branching orders incrementally
void n3::tree3d::getBranchOrders (std::list<int>& orders, 
				  MaGraph const& mag)
{
  if (mag.edges.size() == 0) return;
  MaGraphEdgeMap::const_iterator maxeit = mag.edges.begin();
  double maxlen = maxeit->second.data.size();
  for (MaGraphEdgeMap::const_iterator eit = mag.edges.begin(); 
       eit != mag.edges.end(); ++eit) {
    double len = eit->second.data.size();
    if (len > maxlen) {
      maxeit = eit;
      maxlen = len;
    }
  }
  std::map<int, int> o;
  getBranchOrders(o, mag, maxeit);
  for (std::map<int, int>::const_iterator oit = o.begin(); 
       oit != o.end(); ++oit) orders.push_back(oit->second);
}



void n3::tree3d::getRegionShapeFeatures (flist& feat, Points3 const& body, 
					 Points3 const& surface, 
					 Points3 const* border, 
					 MaGraph const& mag)
{
  // Simple stuff
  feat.push_back(border != NULL? 1.0: 0.0);
  float v = (float)body.size(), a = (float)surface.size();
  if (border != NULL) a += (float)border->size();
  feat.push_back(v);
  feat.push_back(a);
  feat.push_back(getr(pow(a, 1.5), v));
  // Segment length stats
  flist lens;
  for (MaGraph::EdgeMap::const_iterator eit = mag.edges.begin(); 
       eit != mag.edges.end(); ++eit)
    lens.push_back((float)(eit->second.data.size()));
  append(feat, getStats(lens, false));
  double lsum = getSum(lens); 
  feat.push_back(lsum);
  feat.push_back(getr(v, lsum));
  double lmax = getMax(lens, true);
  for (flist::iterator lit = lens.begin(); lit != lens.end(); ++lit) 
    *lit /= lmax;
  append(feat, getHist(lens, 0.0, 1.0, RC_SEGLEN_HIST_BIN, true));
  // Branching order stats
  std::list<int> bOrders;
  getBranchOrders(bOrders, mag);
  feat.push_back(bOrders.size());
  append(feat, getHist(bOrders, 0, RC_BRNCHORDER_HIST_UPPER, 
		       RC_BRNCHORDER_HIST_BIN, true));
  fvec bOrderStats = getStats(bOrders, false);
  for (int i = 1; i < 4; ++i) feat.push_back(bOrderStats[i]); // Skip min
  // Branching angle stats
  std::list<MaGraphNodeMap::const_iterator> nonterminals;
  mag.get_nonterminals(nonterminals);
  feat.push_back(nonterminals.size());
  std::map<int, std::list<double> > bAngles;
  getBranchAngles(bAngles, mag, &nonterminals);
  flist minAngles, maxAngles;
  for (std::map<int, std::list<double> >::iterator bait = bAngles.begin(); 
       bait != bAngles.end(); ++bait) {
    bait->second.sort();
    minAngles.push_back(bait->second.front());
    maxAngles.push_back(bait->second.back());
  }
  append(feat, getHist(minAngles, 0.0, PI, RC_ANGLE_HIST_BIN, true));
  append(feat, getStats(minAngles, false));
  append(feat, getHist(maxAngles, 0.0, PI, RC_ANGLE_HIST_BIN, true));
  append(feat, getStats(maxAngles, false));  
}



void n3::tree3d::getRegionShapeFeatures (flist& feat, Points3 const& body, 
					 Points3 const& surface, 
					 Points3 const* border, 
					 Box3 const* pBodyBox, 
					 LabelImage3::Pointer canvas, 
					 double limbThreshold)
{
  // Medial axis related stuff
  canvas->FillBuffer(0);
  setvs<LabelImage3>(canvas, body, 1);
  // // For debug
  // std::cout << "start computing medial axis" << std::endl;
  // std::cout << pBodyBox << std::endl;
  // // ~ For debug
  canvas = binaryThin(canvas, pBodyBox);
  MaGraph mag(0);
  getMedialAxisGraph(mag, canvas, pBodyBox, MEDIAL_AXIS_CONN);
  // // For debug
  // std::cerr << "done getting medial axis" << std::endl;
  // // ~ For debug
  pruneMedialAxisGraph(mag, limbThreshold);
  // // For debug
  // std::cout << "done pruning medial axis" << std::endl;
  // // ~ For debug
  smoothMedialAxisGraph(mag, 1.0);
  // // For debug
  // std::cout << "done smoothing medial axis" << std::endl;
  // // ~ For debug
  getRegionShapeFeatures(feat, body, surface, border, mag);
}



void n3::tree3d::getRegionIntensityFeatures (flist& feat, 
					     Points3 const& surface, 
					     FloatImage3::Pointer valImage)
{
  flist vals;
  getvs<FloatImage3>(vals, valImage, surface);
  append(feat, getHist(vals, 0.0, 1.0, RC_INT_HIST_BIN, true));
  append(feat, getStats(vals, false));
}
