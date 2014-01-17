#ifndef _n3_cv_ma_h_
#define _n3_cv_ma_h_

#include "struct/graph.h"
#include "util/image_util.h"
#include "util/cv/cv_curve3.h"
#include "itkBinaryThinningImageFilter3D.h"

namespace n3 {

  typedef Graph<Point3, Points3> MaGraph;
  typedef MaGraph::Node<Point3> MaGraphNode;
  typedef MaGraph::Edge<Points3> MaGraphEdge;

  const int MEDIAL_AXIS_CONN = 26;
  
  LabelImage3::Pointer binaryThin (LabelImage3::Pointer im, 
				   Box3 const* roi);

  // Generate medial axis graph
  // Connect is usually 26
  void getMedialAxisGraph (MaGraph& mag, LabelImage3::Pointer im, 
			   Box3 const* roi, int connect);

  // For each branching point, there are three angles
  // Assume binary branching
  // nt: set of non-terminal ids; use 'NULL' to recompute
  void getBranchAngles (std::map<int, std::list<double> >& angles, 
			MaGraph const& mag, std::list<int>* nt);

  // Compute branching orders for branching points (non-terminals)
  // Start from ends of a given edge
  // Branch order starts from 0, and increases by 1
  void getBranchOrders (std::map<int, int>& orders, MaGraph const& mag, 
			int startEdgeID);

};

#endif
