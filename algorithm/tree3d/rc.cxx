#include "rc.h"
using namespace n3;
using namespace n3::tree3d;

void n3::tree3d::getRegionShapeFeatures (flist& feat, Points3 const& body, 
					 Points3 const& surface, 
					 Box3 const& bodyBox, 
					 LabelImage3::Pointer canvas, 
					 bool touchBorder)
{
  feat.push_back(touchBorder? 1.0: 0.0);
  float v = (float)body.size(), a = (float)surface.size();
  feat.push_back(v);
  feat.push_back(a);
  feat.push_back(getr(pow(v, 1.5), a));
  canvas->FillBuffer(0);
  setvs<LabelImage3>(canvas, body, 1);
  MaGraph mag(0);
  getMedialAxisGraph(mag, canvas, &bodyBox, MEDIAL_AXIS_CONN);
  flist lens;
  for (MaGraph::EdgeMap::const_iterator eit = mag.edges.begin(); 
       eit != mag.edges.end(); ++eit)
    lens.push_back((float)(eit->second.data.size()));
  append(feat, getStats(lens, false));
  double lsum = getSum(lens);
  feat.push_back(lsum);
  feat.push_back(getr(v, lsum));
  for (flist::iterator lit = lens.begin(); lit != lens.end(); ++lit) 
    *lit /= getMax(lens, true);
  append(feat, getHist(lens, 0.0, 1.0, RC_SEGLEN_HIST_BIN, true));
  std::list<int> nonterminals;
  mag.get_nonterminals(nonterminals);
  feat.push_back(nonterminals.size());
  std::map<int, std::list<double> > bangles;
  getBranchAngles(bangles, mag, &nonterminals);
  flist minAngles, maxAngles;
  for (std::map<int, std::list<double> >::iterator bait = bangles.begin(); 
       bait != bangles.end(); ++bait) {
    bait->second.sort();
    minAngles.push_back(bait->second.front());
    maxAngles.push_back(bait->second.back());
  }
  append(feat, getHist(minAngles, 0.0, PI, RC_ANGLE_HIST_BIN, true));
  append(feat, getStats(minAngles, false));
  append(feat, getHist(maxAngles, 0.0, PI, RC_ANGLE_HIST_BIN, true));
  append(feat, getStats(maxAngles, false));
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
