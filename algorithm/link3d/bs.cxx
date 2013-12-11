#include "bs.h"
using namespace n3;
using namespace n3::link3d;

/* Get at most maxNodeNum continous (non-branching) nodes */
// Do not consider other criteria
/* Ids are in order */
// Direction == 1: search out edges (go up)
// Direction == 2: search in edges (go down)
void n3::link3d::getContinuousNodes (std::list<int>& ids, int startNodeId, 
				     fBody const& b, int direction, 
				     const int maxNodeNum)
{
  int id = startNodeId;
  if (direction == 1)
    for (int i = 0; i < maxNodeNum; ++i) {
      ids.push_back(id);
      fBody::Node const* pn = &(b.nodes.find(id)->second);
      if (pn->out_edges.size() != 1) break;
      else id = b.edges.find(*(pn->out_edges.begin()))->second.tar_node;
    }
  else if (direction == 2)
    for (int i = 0; i < maxNodeNum; ++i) {
      ids.push_back(id);
      fBody::Node const* pn = &(b.nodes.find(id)->second);
      if (pn->in_edges.size() != 1) break;
      else id = b.edges.find(*(pn->in_edges.begin()))->second.src_node;
    }
  else if (direction == 3) ids.push_back(id);
  else perr("Error: unsupported search direction...");
}



struct IdArea{
  int id;
  float area;
  IdArea (int id, float area) : id(id), area(area) {};
  bool operator < (IdArea const& ia) {return area > ia.area;} // Descending
};



// Get at most maxNodeNum continous (non-branching) nodes
// Ignore regions too large or small (out of +-2 std)
// Ids are in order
// Direction == 1: search out edges (go up)
// Direction == 2: search in edges (go down)
void n3::link3d::getContinuousNodes (std::list<int>& ids, int startNodeId, 
				     fBody const& b, int direction, 
				     const int maxNodeNum, 
				     std::map<int, PointMap> const& rmaps)
{
  const float deviation_rate = 2.0;
  const int maxSliceDiff = 10;
  int id = startNodeId, sdiff = 0;
  bool stop = false;
  std::list<IdArea> ias;
  double mean, std;
  if (direction == 1)
    do {
      fBody::Node const* pn = &(b.nodes.find(id)->second);
      ias.push_back(IdArea(id, rmaps.find(pn->data.slice)->second
			   .find(pn->data.label2)->second.size()));
      if (pn->out_edges.size() != 1 || sdiff == maxSliceDiff) stop = true;
      else {
	id = b.edges.find(*(pn->out_edges.begin()))->second.tar_node;
	++sdiff;
      }
      if (ias.size() == maxNodeNum || (stop && ias.size() > 2)) {
	mean = 0.0;
	for (std::list<IdArea>::const_iterator it = ias.begin(); 
	     it != ias.end(); ++it) mean += (double)it->area;
	mean = getr(mean, ias.size());
	std = 0.0;
	for (std::list<IdArea>::const_iterator it = ias.begin(); 
	     it != ias.end(); ++it) 
	  std += (double)(it->area - mean) * (double)(it->area - mean);
	std = sqrt(getr(std, ias.size()));
	if (fabs(std) > EPSILON) {
	  ias.sort();
	  std::list<IdArea>::iterator it = ias.begin();
	  while (it != ias.end() && ias.size() > 2 && 
		 fabs(it->area - mean) > deviation_rate * std)
	    it = ias.erase(it);
	}
      }
    } while (ias.size() < maxNodeNum && !stop);
  else if (direction == 2) 
    do {
      fBody::Node const* pn = &(b.nodes.find(id)->second);
      ias.push_back(IdArea(id, rmaps.find(pn->data.slice)->second
			   .find(pn->data.label2)->second.size()));
      if (pn->in_edges.size() != 1 || sdiff == maxSliceDiff) stop = true;
      else {
	id = b.edges.find(*(pn->in_edges.begin()))->second.src_node;
	++sdiff;
      }
      if (ias.size() == maxNodeNum || (stop && ias.size() > 2)) {
	mean = 0.0;
	for (std::list<IdArea>::const_iterator it = ias.begin(); 
	     it != ias.end(); ++it) mean += (double)it->area;
	mean = getr(mean, ias.size());
	std = 0.0;
	for (std::list<IdArea>::const_iterator it = ias.begin(); 
	     it != ias.end(); ++it) 
	  std += (double)(it->area - mean) * (double)(it->area - mean);
	std = sqrt(getr(std, ias.size()));
	if (fabs(std) > EPSILON) {
	  ias.sort();
	  std::list<IdArea>::iterator it = ias.begin();
	  while (it != ias.end() && ias.size() > 2 && 
		 fabs(it->area - mean) > deviation_rate * std)
	    it = ias.erase(it);
	}
      }
    } while (ias.size() < maxNodeNum && !stop);
  else if (direction == 3) ias.push_back(IdArea(startNodeId, 0.0));
  else perr("Error: unsupported search direction...");
  for (std::list<IdArea>::const_iterator it = ias.begin(); 
       it != ias.end(); ++it) ids.push_back(it->id);
}



// Fit a 3D line (vx, vy, vz, x0, y0, z0) using centroids
// dz: relative z resolution (1 in z == dz in x or y)
void n3::link3d::getCentroidLine 
(fvec& line, std::list<int> const& ids, fBody const& b, float dz, 
 std::map<int, Point> const& centroids)
// void n3::link3d::getCentroidLine 
// (fvec& line, std::list<int> const& ids, fBody const& b, float dz, 
//  boost::unordered_map<int, Point> const& centroids)
{
  Points3 p3;
  for (std::list<int>::const_iterator it = ids.begin(); it != ids.end(); 
       ++it) {
    Point const* p = &(centroids.find(*it)->second);
    if (p->x >= 0.0 && p->y >= 0.0) 
      p3.push_back(Point3(*p, b.nodes.find(*it)->second
			  .data.slice * dz));
  }
  if (p3.size() > 0) fitLine(line, p3, true);
  else line.resize(6, 0.0);
}



// Fit a 3D line (vx, vy, vz, x0, y0, z0) using points
// dz: relative z resolution (1 in z == dz in x or y)
void n3::link3d::getLine (fvec& line, std::list<int> const& ids, 
			  fBody const& b, float dz, Points const& points)
{
  Points3 p3;
  std::list<int>::const_iterator idit = ids.begin(); 
  Points::const_iterator pit = points.begin(); 
  while (idit != ids.end()) {
    if (pit->x >= 0.0 && pit->y >= 0.0)
      p3.push_back(Point3(*pit, b.nodes.find(*idit)->second
			  .data.slice * dz));
      ++idit;
      ++pit;
  }
  if (p3.size() > 0) fitLine(line, p3, true);
  else line.resize(6, 0.0);
}



// dz: relative z resolution (1 in z == dz in x or y)
// Canvas should be 1 pixel bigger than real image on each side
// However, imageWidth and imageHeight should be actual image size
void n3::link3d::getBodyStat (BodyStat& bs, fBody const& b, float dz, 
			      std::map<int, PointMap> const& rmaps, 
			      cv::Mat& canvas, int imageWidth, 
			      int imageHeight)
{
  bs.label = b.label;
  // Get terminals
  int bottomSlice = rmaps.begin()->first, topSlice = rmaps.rbegin()->first;
  std::list<IntPair> terminals;
  b.get_terminals(terminals, false, bottomSlice, topSlice);
  for (std::list<IntPair>::const_iterator tit = terminals.begin(); 
       tit != terminals.end(); ++tit) 
    bs.terminals[tit->first] = tit->second;
  // Get centroids
  for (fBody::NodeMap::const_iterator nit = b.nodes.begin(); 
       nit != b.nodes.end(); ++nit) {
    int slice = nit->second.data.slice;
    Label l2 = nit->second.data.label2;
    PointMap const* prm = &(rmaps.find(slice)->second);
    Points const* pr = &(prm->find(l2)->second);
    bs.centroids[nit->second.id] = pr->get_centroid();
  }
  // Get terminal continuous node ids
  for (std::map<int, int>::const_iterator tit = bs.terminals.begin(); 
       tit != bs.terminals.end(); ++tit)
    getContinuousNodes(bs.tnodes[tit->first], tit->first, b, 
		       tit->second, 5);
  // Get terminal continous node fitted ellipses
  for (std::map<int, std::list<int> >::const_iterator tnit = 
	 bs.tnodes.begin(); tnit != bs.tnodes.end(); ++tnit) 
    for (std::list<int>::const_iterator nit = tnit->second.begin(); 
	 nit != tnit->second.end(); ++nit) {
      // boost::unordered_map<int, RotBox>::iterator eit = 
      // 	bs.ellipses.find(*nit);
      std::map<int, RotBox>::iterator eit = bs.ellipses.find(*nit);
      if (eit == bs.ellipses.end()) {
	fBody::Node const* pn = &(b.nodes.find(*nit)->second);	
	int slice = pn->data.slice;
	Label l2 = pn->data.label2;
	Points const* pp = &(rmaps.find(slice)->second.find(l2)->second);
	Points uc;
	getContour(uc, canvas, pp, CRCONN, imageWidth, imageHeight, 
		   1, 1, true);
	std::vector<Points> oc;
	getContour(oc, canvas, pp, &uc, 1, 1, true);
	fitEllipse(bs.ellipses[*nit], oc[0]);
      }
    }
  // Get region/ellipse centroid lines
  for (std::map<int, std::list<int> >::const_iterator nit = 
	 bs.tnodes.begin(); nit != bs.tnodes.end(); ++nit) {
    getCentroidLine(bs.rlines[nit->first], nit->second, b, 
		    dz, bs.centroids);
    Points ecp;
    for (std::list<int>::const_iterator it = nit->second.begin(); 
	 it != nit->second.end(); ++it) 
      ecp.push_back(bs.ellipses.find(*it)->second.center);
    getLine(bs.elines[nit->first], nit->second, b, dz, ecp);
  }
}
