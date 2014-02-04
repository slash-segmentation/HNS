#ifndef _n3_tree3d_util_h_
#define _n3_tree3d_util_h_

#include "util/struct_util3.h"
#include "tree2d/tree2d_util.h"

namespace n3 {

  namespace tree3d {

    typedef std::map<LabelPair, std::list<fPixel3> > BoundaryTable3;

    // Used in pre-merging
    Label getMinBoundaryNeighbor (BoundaryTable3 const& bt, Label r0);

    // Do 2 types of merge
    // Input label image will be modified
    LabelImage3::Pointer merge (LabelImage3::Pointer labelImage, 
				FloatImage3::Pointer pbImage, 
				int ath0, int ath1, double pth);

    // -median
    Float getSaliency (std::list<fPixel3> const& pixels);

    // In key <r0, r1> keep r0 < r1
    void getBoundaryTable (BoundaryTable3& bt, PointLabelMap3 const& lmap, 
			   FloatImage3::Pointer valImage);

    // In key <r0, r1> keep r0 < r1
    void getBoundaryTable (BoundaryTable3& bt, tree2d::MergeQueue& mq, 
			   PointLabelMap3 const& lmap, 
			   FloatImage3::Pointer valImage);

    // Assume r01 is greater than any existing region label
    // Use mq = NULL to skip updating it
    // Use removed = NULL to skip collecting removed label pairs
    void updateBoundaryTable (BoundaryTable3& bt, tree2d::MergeQueue* mq, 
			      std::set<LabelPair>* removed, 
			      Label r0, Label r1, Label r01);

    // Merge from rfrom to rto
    void updateBoundaryTable (BoundaryTable3& bt, 
			      PointLabelMap3 const& lmap, 
			      Label rfrom, Label rto);

    // bp: all 0 pixel points
    void getMerges (std::list<tree2d::fMerge>& merges, 
		    PointMap3& cmap, Points3 const& bp, 
		    LabelImage3::Pointer labelImage, 
		    FloatImage3::Pointer valImage);

    Label getMaxLabel (PointLabelMap3 const& lmap);

    // bp: all 0 pixel points
    void getMerges (std::list<tree2d::fMerge>& merges, Points3 const& bp, 
		    LabelImage3::Pointer labelImage, 
		    FloatImage3::Pointer valImage);

    void getMerges (std::list<tree2d::fMerge>& merges, 
		    LabelImage3::Pointer labelImage, 
		    FloatImage3::Pointer valImage);

    void getBoundary (Points3& boundary, Points3 const& contour0, 
		      Points3 const& contour1, 
		      LabelImage3::Pointer canvas);

    // Given whether a leaf node touches image borders
    // Determine whether all nodes touch image borders
    void getNodeBorderIndicators (std::vector<bool>& indicators, 
				  tree2d::fTree const& tree, 
				  PointMap3 const& bmap);

    /* Generate hierarchical region and contour point map */
    template <typename T> void
      getPointMap (PointMap3& rmap, PointMap3& cmap, 
		   LabelImage3::Pointer im, 
		   std::list<tree2d::Merge<T> > const& merges, 
		   bool includeBG, int connect, bool keepSrc)
      {
	PointLabelMap3 lmap;
	getPointMap(rmap, im, true);
	getPointNeighbors(lmap, rmap[BGVAL], im, connect);
	getNeighborPoints(cmap, lmap, true);
	for (typename std::list<tree2d::Merge<T> >::const_iterator it = 
	       merges.begin();it != merges.end(); ++it)
	  merge(rmap, cmap, lmap, it->from0, it->from1, it->to, 
		true, keepSrc);
	if (!includeBG) rmap.erase(BGVAL);
      }


    /* Generate hierarchical region point map */
    template <typename T> void
      getPointMap (PointMap3& rmap, LabelImage3::Pointer im, 
		   std::list<tree2d::Merge<T> > const& merges, 
		   bool includeBG, int connect, bool keepSrc)
      {
	PointMap3 cmap;
	getPointMap(rmap, cmap, im, merges, includeBG, connect, keepSrc);
      }

  };

};

#endif
