#include "cv_ma.h"
using namespace n3;

LabelImage::Pointer n3::binaryThin (LabelImage::Pointer im, 
				    Box const* roi)
{
  typedef itk::BinaryThinningImageFilter<LabelImage, LabelImage> B;
  B::Pointer b = B::New();
  b->SetInput(im);
  if (roi != NULL) 
    b->GetOutput()->SetRequestedRegion(getImageRegion(*roi));
  b->Update();
  return b->GetOutput();
}



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
  Point3 start(-1, -1, -1);
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
  if (start.x < 0 || start.y < 0 || start.z < 0) return;
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



// nt: set of non-terminal const iterators; use 'NULL' to recompute
void n3::getBranchAngles (std::map<int, std::list<double> >& angles, 
			  MaGraph const& mag, 
			  std::list<MaGraphNodeMap::const_iterator>* nt)
{
  const int LN_FIT_PT_NUM = 10;	// # of points to fit a line
  std::list<MaGraphNodeMap::const_iterator> nonterminals;
  if (nt == NULL) {
    mag.get_nonterminals(nonterminals);
    nt = &nonterminals;
  }
  for (std::list<MaGraphNodeMap::const_iterator>::const_iterator 
	 ntit = nt->begin(); 
       ntit != nt->end(); ++ntit) {
    MaGraphNode const* pn = &((*ntit)->second);
    // if (pn->in_edges.size() != 1) 
    //   perr("Unexpected: incoming edge # is not 1...");
    // if (pn->out_edges.size() != 2) 
    //   perr("Unexpected: outgoing edge # is not 2...");
    std::vector<Points3> points;
    points.reserve(pn->in_edges.size() + pn->out_edges.size());
    // Incoming branch
    for (std::list<int>::const_iterator eidit = pn->in_edges.begin(); 
	 eidit != pn->in_edges.end(); ++eidit) {
      points.push_back(Points3());
      int cnt = 0;
      MaGraphEdge const* ie = &(mag.edges.find(*eidit)->second);
      for (Points3::const_reverse_iterator pit = ie->data.rbegin(); 
	   pit != ie->data.rend(); ++pit) {
	if (cnt >= LN_FIT_PT_NUM) break;
	points.back().push_back(*pit);
	++cnt;
      }
    }
    // Outgoing branch    
    for (std::list<int>::const_iterator eit = pn->out_edges.begin(); 
	 eit != pn->out_edges.end(); ++eit) {
      points.push_back(Points3());
      MaGraphEdge const* oe = &(mag.edges.find(*eit)->second);
      int cnt = 0;
      for (Points3::const_iterator pit = oe->data.begin(); 
	   pit != oe->data.end(); ++pit) {
	if (cnt >= LN_FIT_PT_NUM) break;
	points.back().push_back(*pit);
	++cnt;
      }
    }
    // Fit lines
    std::vector<fvec> lines(points.size());
    for (int i = 0; i < points.size(); ++i) 
      fitLine(lines[i], points[i], true);
    // Get angles;
    std::list<double>* pa = &(angles[pn->id]);
    for (std::vector<fvec>::const_iterator lit0 = lines.begin(); 
	 lit0 != lines.end(); ++lit0) {
      std::vector<fvec>::const_iterator lit1 = lit0; ++lit1;
      while (lit1 != lines.end()) {
	pa->push_back(getIncludedAngle(*lit0, *lit1));
	++lit1;
      }
    }
  }
}



// Compute branching orders for branching points (non-terminals)
// Start from ends of a given edge
// Branch order starts from 0, and increases by 1
void n3::getBranchOrders (std::map<int, int>& orders, MaGraph const& mag, 
			  MaGraphEdgeMap::const_iterator steit)
{
  MaGraphEdge const* pe = &(steit->second);
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



// Remove branching edge *beit between branching node *bnit
// And terminal node *tnit
// Remove terminal node *tnit
// If *bnit becomes non-branching node 
// Remove it and connect its neighbor nodes
void removeBranch (MaGraph& mag, MaGraphEdgeMap::iterator beit, 
		   MaGraphNodeMap::iterator bnit, 
		   MaGraphNodeMap::iterator tnit)
{
  mag.remove_node(tnit);
  MaGraphNode* bn = &(bnit->second);
  // Case: n0 <- bn -> n1 ==> n0 -> n1 or n0 <- n1
  if (bn->in_edges.size() == 0 && bn->out_edges.size() == 2) {
    // Keep bne0->data shorter than bne1->data
    MaGraphEdge* bne0 = &(mag.edges.find(bn->out_edges.front())->second);
    MaGraphEdge* bne1 = &(mag.edges.find(bn->out_edges.back())->second);
    if (bne0->data.size() > bne1->data.size()) std::swap(bne0, bne1);
    // // For debug
    // std::cerr << "combine: "
    // 	      << "[" << bne0->id << ": "
    // 	      << bne0->src_node << " - " 
    // 	      << bne0->tar_node << " (" 
    // 	      << bne0->data.size() << ")] "
    // 	      << " + "
    // 	      << "[" << bne1->id << ": "
    // 	      << bne1->src_node << " - " 
    // 	      << bne1->tar_node << " (" 
    // 	      << bne1->data.size() << ")] "
    // 	      << " = ";
    // // ~ For debug
    int nsrc = bne0->tar_node, ntar = bne1->tar_node;
    MaGraphEdgeMap::iterator neit;
    mag.add_edge(nsrc, ntar, neit);
    // Point list order is reverse than edge direction
    bne0->data.pop_front();
    bne0->data.reverse();
    splice(bne0->data, bne1->data, false);
    splice(neit->second.data, bne0->data, false);
    mag.remove_node(bnit);
    // // For debug
    // std::cerr << "[" << neit->second.id << ": "
    // 	      << neit->second.src_node << " - " 
    // 	      << neit->second.tar_node << " (" 
    // 	      << neit->second.data.size() << ")]\n"
    // 	      << std::endl;
    // // ~ For debug
  }
  // Case: n0 <- bn <- n1 ==> n0 <- n1
  else if (bn->in_edges.size() == 1 && bn->out_edges.size() == 1) {
    MaGraphEdge* bne0 = &(mag.edges.find(bn->out_edges.front())->second);
    MaGraphEdge* bne1 = &(mag.edges.find(bn->in_edges.front())->second);
    // // For debug
    // std::cerr << "combine: "
    // 	      << "[" << bne0->id << ": "
    // 	      << bne0->src_node << " - " 
    // 	      << bne0->tar_node << " (" 
    // 	      << bne0->data.size() << ")] "
    // 	      << " + "
    // 	      << "[" << bne1->id << ": "
    // 	      << bne1->src_node << " - " 
    // 	      << bne1->tar_node << " (" 
    // 	      << bne1->data.size() << ")] "
    // 	      << " = ";
    // // ~ For debug
    int nsrc = bne1->src_node, ntar = bne0->tar_node;
    MaGraphEdgeMap::iterator neit;
    mag.add_edge(nsrc, ntar, neit);
    // Point list order is reverse than edge direction
    bne0->data.pop_front();    
    splice(bne1->data, bne0->data, false);
    splice(neit->second.data, bne1->data, false);
    mag.remove_node(bnit);
    // // For debug
    // std::cerr << "[" << neit->second.id << ": "
    // 	      << neit->second.src_node << " - " 
    // 	      << neit->second.tar_node << " (" 
    // 	      << neit->second.data.size() << ")]\n"
    // 	      << std::endl;
    // // ~ For debug
  }
  // Case: n0 -> bn <- n1 ==> n0 <- n1 or n0 -> n1
  else if (bn->in_edges.size() == 2 && bn->out_edges.size() == 0) {
    // Keep bne0->data shorter than bne1->data
    MaGraphEdge* bne0 = &(mag.edges.find(bn->in_edges.front())->second);
    MaGraphEdge* bne1 = &(mag.edges.find(bn->in_edges.back())->second);
    if (bne0->data.size() > bne1->data.size()) std::swap(bne0, bne1);
    // // For debug
    // std::cerr << "combine: "
    // 	      << "[" << bne0->id << ": "
    // 	      << bne0->src_node << " - " 
    // 	      << bne0->tar_node << " (" 
    // 	      << bne0->data.size() << ")] "
    // 	      << " + "
    // 	      << "[" << bne1->id << ": "
    // 	      << bne1->src_node << " - " 
    // 	      << bne1->tar_node << " (" 
    // 	      << bne1->data.size() << ")] "
    // 	      << " = ";
    // // ~ For debug
    int nsrc = bne1->src_node, ntar = bne0->src_node;
    MaGraphEdgeMap::iterator neit;
    mag.add_edge(nsrc, ntar, neit);
    // Point list order is reverse than edge direction
    bne0->data.pop_back();
    bne0->data.reverse();
    splice(bne1->data, bne0->data, false);
    splice(neit->second.data, bne1->data, false);
    mag.remove_node(bnit);
    // // For debug
    // std::cerr << "[" << neit->second.id << ": "
    // 	      << neit->second.src_node << " - " 
    // 	      << neit->second.tar_node << " (" 
    // 	      << neit->second.data.size() << ")]\n"
    // 	      << std::endl;
    // // ~ For debug
  }
  // Do not handle other cases
}



// Recursively remove terminal segments shorter than threshold
// Always remove shortest edge at each iteration
// At least one edge is left
// Important: do not prune terminal segment with terminal node
// That has only out edge because it should be only global source node 
// Removing it cause code difficulty of making another global source node
void n3::pruneMedialAxisGraph (MaGraph& mag, double threshold)
{
  while (mag.edges.size() > 1) {
    std::list<std::pair<MaGraphNodeMap::iterator, int> > terminals;
    mag.get_terminals(terminals);
    MaGraphNodeMap::iterator tnit = mag.nodes.end();
    MaGraphEdgeMap::iterator beit = mag.edges.end(); 
    double minlen = threshold;
    for (std::list<std::pair<MaGraphNodeMap::iterator, 
	   int> >::const_iterator tit = terminals.begin(); 
	 tit != terminals.end(); ++tit) {
      if (tit->second == 2) { // Terminal node has no out edge
	MaGraphEdgeMap::iterator eit = 
	  mag.edges.find(tit->first->second.in_edges.front());
	double len = eit->second.data.size();
	if (len < minlen) {
	  beit = eit;
	  tnit = tit->first;
	  minlen = len;
	}
      }
      else if (tit->second != 1) 
	perr("Unexpected: isolated MaGraphNode found...");
    }
    if (minlen < threshold) {
      // Branching node should then always be source node
      MaGraphNodeMap::iterator bnit = 
	mag.nodes.find(beit->second.src_node);
      // // For debug
      // std::cerr << "remove: " 
      // 		<< "[" << beit->second.id << ": "
      // 		<< beit->second.src_node << " - " 
      // 		<< beit->second.tar_node << " (" 
      // 		<< beit->second.data.size() << ")]" << std::endl;
      // // ~ For debug
      removeBranch(mag, beit, bnit, tnit);
    }
    else break;
  }
}



void n3::smoothMedialAxisGraph (MaGraph& mag, double sigma)
{
  for (MaGraphEdgeMap::iterator eit = mag.edges.begin(); 
       eit != mag.edges.end(); ++eit) {
    Points3 smoothed;
    smoothCurve(smoothed, eit->second.data, sigma, false);
    eit->second.data = smoothed;
  }
}



void n3::writeMedialAxisGraph (const char* fileName, MaGraph const& mag)
{
  std::ofstream os(fileName);
  if (os.is_open()) {
    os << mag << std::endl;
    os.close();
  }
  else perr("Error: cannot create medial axis graph file...");
}



void n3::readMedialAxisGraph (MaGraph& mag, const char* fileName)
{
  std::ifstream is(fileName);
  if (is.is_open()) {
    is >> mag;
    is.close();
  }
  else perr("Error: cannot read medial axis graph file...");
}
