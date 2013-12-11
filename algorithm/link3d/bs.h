#ifndef _n3_link3d_bs_h_
#define _n3_link3d_bs_h_

#include "util/struct_util.h"
#include "util/stat_util.h"
#include "link3d.h"
#include "util/cv/cv_contour.h"
#include "util/cv/cv_curve3.h"

namespace n3 {

  namespace link3d {

    struct BodyStat {

      Label label;
      /* {(node_id, terminal_type)} */
      /* Terminal type: 0 unknown, 1: no in edge, 2 no out edge, 3 none */
      /* Exclude terminals on top/bottom slice */
      std::map<int, int> terminals;
      /* boost::unordered_map<int, Point> centroids; */
      std::map<int, Point> centroids;
      
      /* Terminal continuous nodes: tnodes[term_id] = node_ids */
      std::map<int, std::list<int> > tnodes;
      /* ellipses[node_id] = ebox */
      /* boost::unordered_map<int, RotBox> ellipses; */
      std::map<int, RotBox> ellipses;
      /* Region centroid lines: 
	 rlines[term_id] = (vx, vy, vz, x0, y0, z0) */
      std::map<int, fvec> rlines;
      /* Ellipse centroid lines: 
         elines[term_id] = (vx, vy, vz, x0, y0, z0) */
      std::map<int, fvec> elines;

      void print (std::ostream& os) const {
      	os << "Label = " << label << "\n";
      	os << "Terminals = {";
      	std::map<int, int>::const_iterator tit = terminals.begin();
      	while (true) {
      	  os << "(" << tit->first << ", " << tit->second << ")";
      	  ++tit;
      	  if (tit != terminals.end()) os << ", ";
      	  else {
      	    os << "}\n";
      	    break;
      	  }
      	}
      	os << "Centroids = {";
      	/* boost::unordered_map<int, Point>::const_iterator cit = */
      	/*   centroids.begin(); */
      	std::map<int, Point>::const_iterator cit = centroids.begin();
      	while (true) {
      	  os << "(" << cit->first << ", " << cit->second.x
      	     << ", " << cit->second.y << ")";
      	  ++cit;
      	  if (cit != centroids.end()) os << ", ";
      	  else {
      	    os << "}\n";
      	    break;
      	  }
      	}
      	os << "Terminal nodes = {";
      	std::map<int, std::list<int> >::const_iterator nit =
      	  tnodes.begin();
      	while (true) {
      	  os << "(" << nit->first << ": ";
      	  std::list<int>::const_iterator it = nit->second.begin();
      	  while (true) {
      	    os << *it;
      	    ++it;
      	    if (it != nit->second.end()) os << ", ";
      	    else {
      	      os << ")";
      	      break;
      	    }
      	  }
      	  ++nit;
      	  if (nit != tnodes.end()) os << ", ";
      	  else {
      	    os << "}\n";
      	    break;
      	  }
      	}
      	os << "Ellipses = {";
      	/* boost::unordered_map<int, RotBox>::const_iterator eit = */
      	/*   ellipses.begin(); */
      	std::map<int, RotBox>::const_iterator eit = ellipses.begin();
      	while (true) {
      	  os << "(" << eit->first << ", " << eit->second.center.x << ", "
      	     << eit->second.center.y << ", " << eit->second.width << ", "
      	     << eit->second.height << ")";
      	  ++eit;
      	  if (eit != ellipses.end()) os << ", ";
      	  else {
      	    os << "}\n";
      	    break;
      	  }
      	}
      }

      friend std::ostream& operator << (std::ostream& os, 
					BodyStat const& bs) {
	os << bs.label << " " << bs.terminals.size() << " ";
	for (std::map<int, int>::const_iterator it = bs.terminals.begin(); 
	     it != bs.terminals.end(); ++it) 
	  os << it->first << " " << it->second << " ";
	os << bs.centroids.size() << " ";
	/* for (boost::unordered_map<int, Point>::const_iterator it =  */
	/*        bs.centroids.begin(); it != bs.centroids.end(); ++it)  */
	for (std::map<int, Point>::const_iterator it = 
	       bs.centroids.begin(); it != bs.centroids.end(); ++it) 
	  os << it->first << " " << it->second << " ";
	os << bs.tnodes.size() << " ";
	for (std::map<int, std::list<int> >::const_iterator it0 = 
	       bs.tnodes.begin(); it0 != bs.tnodes.end(); ++it0) {
	  os << it0->first << " " << it0->second.size() << " ";
	  for (std::list<int>::const_iterator it1 = it0->second.begin(); 
	       it1 != it0->second.end(); ++it1) os << *it1 << " ";
	}
	os << bs.ellipses.size() << " ";
	/* for (boost::unordered_map<int, RotBox>::const_iterator it =  */
	/*        bs.ellipses.begin(); it != bs.ellipses.end(); ++it)  */
	for (std::map<int, RotBox>::const_iterator it = 
	       bs.ellipses.begin(); it != bs.ellipses.end(); ++it) 
	  os << it->first << " " << it->second << " ";
	os << bs.rlines.size() << " ";
	for (std::map<int, fvec>::const_iterator it0 = bs.rlines.begin(); 
	     it0 != bs.rlines.end(); ++it0) {
	  os << it0->first << " " << it0->second.size() << " ";
	  for (fvec::const_iterator it1 = it0->second.begin(); 
	       it1 != it0->second.end(); ++it1) os << *it1 << " ";
	}
	os << bs.elines.size() << " ";
	for (std::map<int, fvec>::const_iterator it0 = bs.elines.begin(); 
	     it0 != bs.elines.end(); ++it0) {
	  os << it0->first << " " << it0->second.size() << " ";
	  for (fvec::const_iterator it1 = it0->second.begin(); 
	       it1 != it0->second.end(); ++it1) os << *it1 << " ";
	}
	return os;
      }


      friend std::istream& operator >> (std::istream& is, BodyStat& bs) {
	is >> bs.label;
	int n;
	is >> n;
	for (int i = 0; i < n; ++i) {
	  int key;
	  is >> key;
	  is >> bs.terminals[key];
	}
	is >> n;
	for (int i = 0; i < n; ++i) {
	  int key;
	  is >> key;
	  is >> bs.centroids[key];
	}
	is >> n;
	for (int i = 0; i < n; ++i) {
	  int key;
	  is >> key;
	  std::list<int>* p = &(bs.tnodes[key]);
	  int m;
	  is >> m;
	  for (int j = 0; j < m; ++j) {
	    int data;
	    is >> data;
	    p->push_back(data);
	  }
	}
	is >> n;
	for (int i = 0; i < n; ++i) {
	  int key;
	  is >> key;
	  is >> bs.ellipses[key];	  
	}
	is >> n;
	for (int i = 0; i < n; ++i) {
	  int key;
	  is >> key;
	  fvec* p = &(bs.rlines[key]);
	  int m;
	  is >> m;
	  p->reserve(m);
	  for (int j = 0; j < m; ++j) {
	    float data;
	    is >> data;
	    p->push_back(data);
	  }
	}
	is >> n;
	for (int i = 0; i < n; ++i) {
	  int key;
	  is >> key;
	  fvec* p = &(bs.elines[key]);
	  int m;
	  is >> m;
	  p->reserve(m);
	  for (int j = 0; j < m; ++j) {
	    float data;
	    is >> data;
	    p->push_back(data);
	  }
	}
	return is;
      }
    
    };

    /* Get at most maxNodeNum continous (non-branching) nodes */
    // Do not consider other criteria
    /* Ids are in order */
    // Direction == 1: search out edges (go up)
    // Direction == 2: search in edges (go down)
    void getContinuousNodes (std::list<int>& ids, int startNodeId, 
			     fBody const& b, int direction, 
			     const int maxNodeNum);

    // Get at most maxNodeNum continous (non-branching) nodes
    // Ignore regions too large or small (out of +-2 std)
    // Ids are in order
    // Direction == 1: search out edges (go up)
    // Direction == 2: search in edges (go down)
    void getContinuousNodes (std::list<int>& ids, int startNodeId, 
			     fBody const& b, int direction, 
			     const int maxNodeNum, 
			     std::map<int, PointMap> const& rmaps);

    // Fit a 3D line (vx, vy, vz, x0, y0, z0) using centroids
    // dz: relative z resolution (1 in z == dz in x or y)
    void getCentroidLine (fvec& line, std::list<int> const& ids, 
			  fBody const& b, float dz, 
			  std::map<int, Point> const& centroids);
    /* void getCentroidLine (fvec& line, std::list<int> const& ids,  */
    /* 			  fBody const& b, float dz,  */
    /* 			  boost::unordered_map<int, Point> const&  */
    /* 			  centroids); */

    // Fit a 3D line (vx, vy, vz, x0, y0, z0) using points
    // dz: relative z resolution (1 in z == dz in x or y)
    void getLine (fvec& line, std::list<int> const& ids, 
		  fBody const& b, float dz, Points const& points);

    // dz: relative z resolution (1 in z == dz in x or y)
    // Canvas should be 1 pixel bigger than real image on each side
    // However, imageWidth and imageHeight should be actual image size
    void getBodyStat (BodyStat& bs, fBody const& b, float dz, 
		      std::map<int, PointMap> const& rmaps, 
		      cv::Mat& canvas, int imageWidth, int imageHeight);

  };

};

#endif
