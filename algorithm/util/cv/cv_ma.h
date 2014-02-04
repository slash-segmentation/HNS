#ifndef _n3_cv_ma_h_
#define _n3_cv_ma_h_

#include "struct/graph.h"
#include "util/image_util.h"
#include "util/cv/cv_curve3.h"
#include "itkBinaryThinningImageFilter.h"
#include "itkBinaryThinningImageFilter3D.h"

namespace n3 {

  typedef Graph<Point3, Points3> MaGraph;
  typedef MaGraph::NodeMap MaGraphNodeMap;
  typedef MaGraph::EdgeMap MaGraphEdgeMap;
  typedef MaGraphNodeMap::mapped_type MaGraphNode;
  typedef MaGraphEdgeMap::mapped_type MaGraphEdge;

  const int MEDIAL_AXIS_CONN = 26;

  LabelImage::Pointer binaryThin (LabelImage::Pointer im, 
				  Box const* roi);
  
  LabelImage3::Pointer binaryThin (LabelImage3::Pointer im, 
				   Box3 const* roi);

  // Generate medial axis graph
  // Connect is usually 26
  void getMedialAxisGraph (MaGraph& mag, LabelImage3::Pointer im, 
			   Box3 const* roi, int connect);

  // nt: set of non-terminal ids; use 'NULL' to recompute
  void getBranchAngles (std::map<int, std::list<double> >& angles, 
			MaGraph const& mag, std::list<int>* nt);

  // For each branching point, there are three angles
  // Assume binary branching
  // nt: set of non-terminal const iterators; use 'NULL' to recompute
  void getBranchAngles (std::map<int, std::list<double> >& angles, 
			MaGraph const& mag, 
			std::list<MaGraphNodeMap::const_iterator>* nt);

  // Compute branching orders for branching points (non-terminals)
  // Start from ends of a given edge
  // Branch order starts from 0, and increases by 1
  void getBranchOrders (std::map<int, int>& orders, MaGraph const& mag, 
			MaGraphEdgeMap::const_iterator steit);

  // Recursively remove terminal segments shorter than threshold
  // Always remove shortest edge at each iteration
  // At least one edge is left
  // Important: do not prune terminal segment with terminal node
  // That has only out edge because it should be only global source node 
  // Removing it cause code difficulty of making another global source node
  void pruneMedialAxisGraph (MaGraph& mag, double threshold);

  void smoothMedialAxisGraph (MaGraph& mag, double sigma = 1.0);

  void writeMedialAxisGraph (const char* fileName, MaGraph const& mag);

  void readMedialAxisGraph (MaGraph& mag, const char* fileName);

};

#endif
