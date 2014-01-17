#include "cv_ma.h"
using namespace n3;

LabelImage3::Pointer n3::binaryThin (LabelImage3::Pointer im, 
				     Box3 const* roi)
{
  typedef itk::BinaryThinningImageFilter3D<LabelImage3, LabelImage3> B;
  B::Pointer b = B::New();
  b->SetInput(im);
  if (roi != NULL) 
    b->GetOutput()->SetRequestedRegion(getImageRegion(*roi));
  b->Update();
  return b->GetOutput();
}



// Connect is usually 26
Point3 getNearestTerminal (Point3 const& start, LabelImage3::Pointer im, 
			   int connect)
{
  Points3 nb;
  getNeighbors<LabelImage3>(nb, start, im, connect, 1);  
  if (nb.size() <= 1) return start; // Starting point is a terminal
  else { // Starting point is not a terminal
    Points3 paintBack;
    std::set<Point3> openSet;    
    openSet.insert(start);
    while (!openSet.empty()) {
      Point3 const* pp = &(*openSet.begin());
      setv<LabelImage3>(im, *pp, 2);
      paintBack.push_back(*pp);
      nb.clear();
      getNeighbors<LabelImage3>(nb, *pp, im, connect, 1);
      if (nb.empty()) {
	setvs<LabelImage3>(im, paintBack, 1);
	return *pp;
      }
      else {
	openSet.erase(openSet.begin());
	for (Points3::const_iterator nit = nb.begin(); nit != nb.end(); 
	     ++nit) openSet.insert(*nit);
      }
    }
  }
}



// Given start point and its next point, find a segment
// Return true if end point is a terminal
// Connect is usually 26
bool getSegment (Point3& endPoint, Points3& segment, 
		 Point3 const& startPoint, Point3 const& nextPoint, 
		 LabelImage3::Pointer im, int connect)
{
  segment.push_back(startPoint);
  setv<LabelImage3>(im, startPoint, 2); // Visited status = 2
  endPoint = nextPoint;
  Points3 nb;
  getNeighbors<LabelImage3>(nb, endPoint, im, connect, 1);
  while (nb.size() == 1) {
    segment.push_back(endPoint);
    setv<LabelImage3>(im, endPoint, 2);
    endPoint = nb.front();
    nb.clear();
    getNeighbors<LabelImage3>(nb, endPoint, im, connect, 1);
  }
  segment.push_back(endPoint);
  setv<LabelImage3>(im, endPoint, 2);
  return nb.empty();
}



// Generate medial axis graph
// Connect is usually 26
void n3::getMedialAxisGraph (MaGraph& mag, LabelImage3::Pointer im, 
			     Box3 const* roi, int connect)
{
  Point3 start;
  // Find first terminal point
  if (roi == NULL) {
    for (itk::ImageRegionConstIteratorWithIndex<LabelImage3> 
	   iit(im, im->GetLargestPossibleRegion()); !iit.IsAtEnd(); ++iit)
      if (iit.Get() == 1) { // Original status = 1
	start = i2p(iit.GetIndex());
	break;
      }
  }
  else {
    for (int x = roi->start.x; x < roi->start.x + roi->width; ++x) 
      for (int y = roi->start.y; y < roi->start.y + roi->height; ++y) 
	for (int z = roi->start.z; z < roi->start.z + roi->deep; ++z) 
	  if (getv<LabelImage3>(im, x, y, z) == 1) {
	    start.x = x;
	    start.y = y;
	    start.z = z;
	    break;
	  }
  }
  start = getNearestTerminal(start, im, connect);
  // Build medial axis graph
  std::set<std::pair<int, Point3> > openSet;
  openSet.insert(std::make_pair(mag.add_node(start), start));
  while (!openSet.empty()) {
    std::pair<int, Point3> pa = *openSet.begin();
    openSet.erase(openSet.begin());
    Points3 nb;
    getNeighbors<LabelImage3>(nb, pa.second, im, connect, 1);
    setvs<LabelImage3>(im, nb, 2); // To resolve connectivity issue
    for (Points3::const_iterator nit = nb.begin(); nit != nb.end(); 
	 ++nit) {
      int nid = mag.add_node();	// For ending point
      int eid = mag.add_edge(pa.first, nid);
      Point3* pep = &(mag.nodes.find(nid)->second.data);
      bool isTerminal = getSegment(*pep, mag.edges.find(eid)->second.data, 
				   pa.second, *nit, im, MEDIAL_AXIS_CONN);
      if (!isTerminal) openSet.insert(std::make_pair(nid, *pep));
    }
  }
}



// For each branching point, there are three angles
// Assume binary branching
// nt: set of non-terminal ids; use 'NULL' to recompute
void n3::getBranchAngles (std::map<int, std::list<double> >& angles, 
			  MaGraph const& mag, std::list<int>* nt)
{
  const int LN_FIT_PT_NUM = 10;	// # of points to fit a line
  std::list<int> nonterminals;
  if (nt == NULL) {
    mag.get_nonterminals(nonterminals);
    nt = &nonterminals;
  }
  for (std::list<int>::const_iterator ntit = nt->begin(); 
       ntit != nt->end(); ++ntit) {
    MaGraphNode const* pn = &(mag.nodes.find(*ntit)->second);
    if (pn->in_edges.size() != 1) 
      perr("Unexpected: incoming edge # is not 1...");
    if (pn->out_edges.size() != 2) 
      perr("Unexpected: outgoing edge # is not 2...");
    std::vector<Points> points;
    points.reserve(3);
    // Incoming branch
    int cnt = 0;
    points.push_back(Points3());
    MaGraphEdge const* ie = 
      &(mag.edges.find(pn->in_edges.front())->second);
    for (Points3::const_reverse_iterator pit = ie->data.rbegin(); 
	 pit != ie->data.rend(); ++pit) {
      if (cnt >= LN_FIT_PT_NUM) break;
      points.back().push_back(*pit);
      ++cnt;
    }
    // Outgoing branch    
    for (std::list<int>::const_iterator eit = pn->out_edges.begin(); 
	 eit != pn->out_edges.end(); ++eit) {
      MaGraphEdge const* oe = &(mag.edges.find(*eit)->second);
      points.push_back(Points3());
      cnt = 0;
      for (Points3::const_iterator pit = oe->data.begin(); 
	   pit != oe->data.end(); ++pit) {
	if (cnt >= LN_FIT_PT_NUM) break;
	points.back().push_back(*pit);
	++cnt;
      }
    }
    // Fit lines
    std::vector<fvec> lines(3);
    for (int i = 0; i < 3; ++i) fitLine(lines[i], points[i], true);
    // Get angles;
    std::list<double>* pa = &(angles[*ntit]);
    pa->push_back(getIncludedAngle(lines[0], lines[1]));
    pa->push_back(getIncludedAngle(lines[0], lines[2]));
    pa->push_back(getIncludedAngle(lines[1], lines[2]));
  }
}



// Compute branching orders for branching points (non-terminals)
// Start from ends of a given edge
// Branch order starts from 0, and increases by 1
void n3::getBranchOrders (std::map<int, int>& orders, MaGraph const& mag, 
			  int startEdgeID)
{
  MaGraphEdge const* pe = &(mag.edges.find(startEdgeID)->second);
  std::set<int> openSet;
  orders[pe->src_node] = 0;
  openSet.insert(pe->src_node);
  orders[pe->tar_node] = 0;
  openSet.insert(pe->tar_node);
  while (!openSet.empty()) {
    int nid = *(openSet.begin());
    openSet.erase(openSet.begin());
    int order = orders.find(nid)->second;
    MaGraphNode const* pn = &(mag.nodes.find(nid)->second);
    for (std::list<int>::const_iterator eit = pn->in_edges.begin(); 
	 eit != pn->in_edges.end(); ++eit) {
      pe = &(mag.edges.find(*eit)->second);
      int snid = pe->src_node;
      if (!mag.nodes.find(snid)->second.in_edges.empty() 
	  && orders.count(snid) == 0) {
	orders[snid] = order + 1;
	openSet.insert(snid);
      }
    }
    for (std::list<int>::const_iterator eit = pn->out_edges.begin(); 
	 eit != pn->out_edges.end(); ++eit) {
      pe = &(mag.edges.find(*eit)->second);
      int tnid = pe->tar_node;
      if (!mag.nodes.find(tnid)->second.out_edges.empty() 
	  && orders.count(tnid) == 0) {
	orders[tnid] = order + 1;
	openSet.insert(tnid);
      }
    }
  }
}
