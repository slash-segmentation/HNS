#ifndef _n3_struct_util3_h_
#define _n3_struct_util3_h_

#include "struct_util.h"

namespace n3 {

  typedef std::map<Label, Points3> PointMap3;
  typedef std::map<LabelPair, Points3> LabelPairPointMap3;
  typedef std::map<Point3, LabelSet> PointLabelMap3;

  void getPoints (Points3& p, LabelImage3::Pointer im, Label label);

  void getPointMap (PointMap3& rmap, LabelImage3::Pointer im, 
		    bool includeBG);

  void getPointNeighbors (PointLabelMap3& lmap, Points3 const& points, 
			  LabelImage3::Pointer im, int connect);

  // Set sort to true for faster merge
  void getNeighborPoints (PointMap3& cmap, PointLabelMap3 const& lmap, 
			  bool sort);

  // Generate flat rmap and cmap
  // Set sort to true for faster merge
  void getPointMap (PointMap3& rmap, PointLabelMap3& lmap, PointMap3& cmap, 
		    LabelImage3::Pointer im, int connect, 
		    bool includeBG, bool sort);

  // c2rp: contour points becoming region points due to merging
  // Since lmap's keys are sorted, c2rp is sorted
  void merge (Points3* c2rp, PointLabelMap3& lmap, Label r0, Label r1, 
	      Label r01);

  // Set sort to true for faster merge; cmap must be sorted
  void merge (Points3* c2rp, PointMap3& cmap, PointLabelMap3& lmap, 
	      Label r0, Label r1, Label r01, bool sort, bool keepSrc);

  // Set sort to true for faster contour merge; cmap must be sorted
  void merge (PointMap3& rmap, PointMap3& cmap, PointLabelMap3& lmap, 
	      Label r0, Label r1, Label r01, bool sort, bool keepSrc);

  // Set sort to true for faster contour merge; cmap must be sorted
  // Also combine border points in bmap if any
  void merge (PointMap3& rmap, PointMap3& cmap, PointLabelMap3& lmap, 
	      PointMap3& bmap, Label r0, Label r1, Label r01, 
	      bool sort, bool keepSrc);

  // Assume merging rfrom to rto
  // Used in pre-merging
  void merge (PointMap3& rmap, PointLabelMap3& lmap, 
	      Label rfrom, Label rto);

  // Get overlapping region label pairs
  void getOverlaps (std::list<LabelPair>& overlaps, 
		    LabelImage3::Pointer im0, LabelImage3::Pointer im1, 
		    bool includeBG0, bool includeBG1);

  unsigned int getOverlap (Points3 const& p0, Points3 const& p1, 
			   LabelImage3::Pointer canvas);

  void getOverlaps (std::set<Label>& overlaps, Points3 const& srcRegion, 
		    LabelImage3::Pointer refImage, bool includeRefBG);

  // Collect all points on image borders
  // Output points are supposed to be in x-y-z-ascending order
  void getBorderPoints (PointMap3& bmap, LabelImage3::Pointer image, 
			bool includeBG);

  /* Find overlaps on ref image for all regions on src image */
  template <typename TCount> void 
    getOverlaps (std::map<LabelPair, TCount>& overlaps, 
		 LabelImage3::Pointer srcImage, 
		 LabelImage3::Pointer refImage, 
		 bool includeSrcBG, bool includeRefBG) 
    {
      itk::ImageRegionConstIterator<LabelImage3> 
	sit(srcImage, srcImage->GetLargestPossibleRegion()), 
	rit(refImage, refImage->GetLargestPossibleRegion());
      while (!(sit.IsAtEnd() || rit.IsAtEnd())) {
	Label l0 = sit.Get(), l1 = rit.Get();
	if ((l0 != BGVAL || includeSrcBG) && 
	    (l1 != BGVAL || includeRefBG)) {
	  LabelPair k = std::make_pair(l0, l1);
	  typename std::map<LabelPair, TCount>::iterator oit = 
	    overlaps.find(k);
	  if (oit != overlaps.end()) ++(oit->second);
	  else overlaps[k] = 1;
	}
	++sit;
	++rit;
      }
    }


  /* Find overlaps on ref image for a region on src image */
  template <typename TCount> void 
    getOverlaps (std::map<Label, TCount>& overlaps, 
		 Points3 const& srcRegion, LabelImage3::Pointer refImage, 
		 bool includeRefBG)
    {
      for (Points3::const_iterator pit = srcRegion.begin(); 
	   pit != srcRegion.end(); ++pit) {
	Label ref = getv<LabelImage3>(refImage, pit->x, pit->y);
	if (ref != BGVAL || includeRefBG) {
	  typename std::map<Label, TCount>::iterator oit = 
	    overlaps.find(ref);
	  if (oit != overlaps.end()) ++(oit->second);
	  else overlaps[ref] = 1;
	}
      }
    }


  /* Remove p2r from src */
  template <typename TImage> void 
    remove (Points3& src, Points3 const& p2r, 
	    typename TImage::Pointer canvas)
    {
      canvas->FillBuffer(0);
      setvs<TImage>(canvas, p2r, 1);
      Points3::iterator sit = src.begin(); 
      while (sit != src.end())
	if (getv<TImage>(canvas, *sit) == 1) 
	  sit = src.erase(sit);
	else ++sit;
    }

};

#endif
