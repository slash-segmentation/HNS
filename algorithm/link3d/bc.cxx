#include "bc.h"
using namespace n3;
using namespace n3::link3d;

void n3::link3d::getAdvancedGeometry (flist& feat, int id0, int id1, 
				      std::map<int, PointMap> const& rmaps, 
				      BodyStat& bs0, BodyStat& bs1, 
				      fBody const& b0, fBody const& b1, 
				      float dz, cv::Mat& canvas, 
				      int imageWidth, int imageHeight)
{
  std::map<int, int>::const_iterator tit0 = bs0.terminals.find(id0);
  std::map<int, int>::const_iterator tit1 = bs1.terminals.find(id1);
  fvec rline0, rline1, eline0, eline1;
  if (tit0 != bs0.terminals.end()) { // Node 0 is a terminal
    fvec const* prl = &(bs0.rlines.find(tit0->first)->second);
    rline0.reserve(3);
    rline0.push_back((*prl)[0]);
    rline0.push_back((*prl)[1]);
    rline0.push_back((*prl)[2]);
    fvec const* pcl = &(bs0.elines.find(tit0->first)->second);
    eline0.reserve(3);
    eline0.push_back((*pcl)[0]);
    eline0.push_back((*pcl)[1]);
    eline0.push_back((*pcl)[2]);
  }
  else { // Node 0 is not a terminal
    std::list<int> cnodes;
    getContinuousNodes(cnodes, id0, b0, 2, 5, rmaps);
    getCentroidLine(rline0, cnodes, b0, dz, bs0.centroids);
    // Points ecp;
    // for (std::list<int>::const_iterator nit = cnodes.begin(); 
    // 	 nit != cnodes.end(); ++nit) {
    //   boost::unordered_map<int, RotBox>::const_iterator eit = 
    // 	bs0.ellipses.find(*nit);
    //   if (eit != bs0.ellipses.end()) ecp.push_back(eit->second.center);
    //   else {
    // 	fBody::Node const* pn = &(b0.nodes.find(*nit)->second);
    // 	int slice = pn->data.slice;
    // 	Label l2 = pn->data.label2;
    // 	Points const* pp = &(rmaps.find(slice)->second.find(l2)->second);
    // 	Points uc;
    // 	getContour(uc, canvas, pp, CRCONN, imageWidth, imageHeight, 
    // 		   1, 1, true);
    // 	std::vector<Points> oc;
    // 	getContour(oc, canvas, pp, &uc, 1, 1, true);
    // 	RotBox* pe = &(bs0.ellipses[*nit]);
    // 	fitEllipse(*pe, oc[0]);
    // 	ecp.push_back(pe->center);
    //   }
    // }
    // getLine(eline0, cnodes, b0, dz, ecp);
  }
  if (tit1 != bs1.terminals.end()) { // Node 1 is a terminal
    fvec const* prl = &(bs1.rlines.find(tit1->first)->second);
    rline1.reserve(3);
    rline1.push_back((*prl)[0]);
    rline1.push_back((*prl)[1]);
    rline1.push_back((*prl)[2]);
    fvec const* pcl = &(bs1.elines.find(tit1->first)->second);
    eline1.reserve(3);
    eline1.push_back((*pcl)[0]);
    eline1.push_back((*pcl)[1]);
    eline1.push_back((*pcl)[2]);
  }
  else { // Node 1 is not a terminal
    std::list<int> cnodes;
    getContinuousNodes(cnodes, id1, b1, 1, 5);
    getCentroidLine(rline1, cnodes, b1, dz, bs1.centroids);
    // Points ecp;
    // for (std::list<int>::const_iterator nit = cnodes.begin(); 
    // 	 nit != cnodes.end(); ++nit) {
    //   boost::unordered_map<int, RotBox>::const_iterator eit = 
    // 	bs1.ellipses.find(*nit);
    //   if (eit != bs1.ellipses.end()) ecp.push_back(eit->second.center);
    //   else {
    // 	fBody::Node const* pn = &(b1.nodes.find(*nit)->second);
    // 	int slice = pn->data.slice;
    // 	Label l2 = pn->data.label2;
    // 	Points const* pp = &(rmaps.find(slice)->second.find(l2)->second);
    // 	Points uc;
    // 	getContour(uc, canvas, pp, CRCONN, imageWidth, imageHeight, 
    // 		   1, 1, true);
    // 	std::vector<Points> oc;
    // 	getContour(oc, canvas, pp, &uc, 1, 1, true);
    // 	RotBox* pe = &(bs1.ellipses[*nit]);
    // 	fitEllipse(*pe, oc[0]);
    // 	ecp.push_back(pe->center);
    //   }
    // }
    // getLine(eline1, cnodes, b1, dz, ecp);
  }
  feat.push_back(getIncludedAngle(rline0[0], rline0[1], rline0[2], 
				  0.0, 0.0, -1.0));
  feat.push_back(getIncludedAngle(rline1[0], rline1[1], rline1[2], 
				  0.0, 0.0, 1.0));
  feat.push_back(getIncludedAngle(rline0[0], rline0[1], rline0[2], 
				  rline1[0], rline1[1], rline1[2]));
  // feat.push_back(getIncludedAngle(eline0[0], eline0[1], eline0[2], 
  // 				  0.0, 0.0, -1.0));
  // feat.push_back(getIncludedAngle(eline1[0], eline1[1], eline1[2], 
  // 				  0.0, 0.0, 1.0));
  // feat.push_back(getIncludedAngle(eline0[0], eline0[1], eline0[2], 
  // 				  eline1[0], eline1[1], eline1[2]));
}
