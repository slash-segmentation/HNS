#ifndef _n3_struct_util_h_
#define _n3_struct_util_h_

#include "n3_util.h"
#include "image_util.h"

namespace n3 {

  typedef std::list<Label> Labels;  
  typedef std::set<Label> LabelSet;
  typedef std::map<Label, Points> PointMap;
  typedef std::map<LabelPair, Points> LabelPairPointMap;
  typedef std::map<Point, LabelSet> PointLabelMap;
  
  // Get points with given label
  void getPoints (Points& p, LabelImage::Pointer im, Label label);

  // Get union of two points
  void getUnionPoints (Points& p, Points const& p0, Points const& p1, 
		       LabelImage::Pointer canvas);

  // Generate flat region point map
  void getPointMap (PointMap& rmap, LabelImage::Pointer im, 
		    bool includeBG);

  // Return rmap; generate if none
  // Read in label image if none
  PointMap* getPointMap (std::map<int, PointMap>& rmaps, 
			 std::map<int, LabelImage::Pointer>& images, 
			 std::map<int, const char*> const& imageNames, 
			 int slice);

  // Get neighbhor region label for each contour point
  void getPointNeighbors (PointLabelMap& lmap, Points const& points, 
			  LabelImage::Pointer im, int connect = CRCONN);

  //----------------------------------------------------------------

  /* // Get contour point for each region label */
  /* void getNeighborPoints (PointMap& cmap, PointLabelMap const& lmap); */

  // Set sort to true for faster merge
  void getNeighborPoints (PointMap& cmap, PointLabelMap const& lmap, 
			  bool sort);

  // Generate flat rmap and cmap
  // Set sort to true for faster merge
  void getPointMap (PointMap& rmap, PointLabelMap& lmap, 
		    PointMap& cmap, LabelImage::Pointer im, 
		    int connect, bool includeBG, bool sort);

  // c2rp: contour points becoming region points due to merging
  void merge (Points* c2rp, PointLabelMap& lmap, Label r0, 
	      Label r1, Label r01);

  // Set sort to true for faster merge; cmap must be sorted
  void merge (Points* c2rp, PointMap& cmap, PointLabelMap& lmap, 
	      Label r0, Label r1, Label r01, bool sort, bool keepSrc, 
	      LabelImage::Pointer canvas);

  // Set sort to true for faster contour merge; cmap must be sorted
  void merge (PointMap& rmap, PointMap& cmap, PointLabelMap& lmap, 
	      Label r0, Label r1, Label r01, bool sort, bool keepSrc, 
	      LabelImage::Pointer canvas);

  /* // Only generate merged region and contour points */
  /* // Do not modify any other structure */
  /* void merge (Points& r01, Points& c01, Label l0, Label l1,  */
  /* 	      Points const& r0, Points const& r1, Points const& c0,  */
  /* 	      Points const& c1, PointLabelMap const& lmap); */

  /* // c2rp: contour points to remove (becoming region points) */
  /* void merge (Points& c2rp, PointMap& cmap, PointLabelMap& lmap,  */
  /* 	      Label r0, Label r1, Label r01); */

  /* // Merge r0 and r1 to r01 */
  /* // rpmap: region point map (to be appended by inside points of r01) */
  /* // rpmap: also to be appended by boundary points between r0/r1 */
  /* // bpmap: contour point map (to be appended by contour points of r01) */
  /* // bplmap: boundary point assignment map (to be changed) */
  /* // bplmap: remove all r0/r1 labels, insert r01 labels */
  /* void merge (PointMap& rmap, PointMap& cmap, PointLabelMap& lmap,  */
  /* 	      Label r0, Label r1, Label r01); */

  //----------------------------------------------------------------

  // Get overlapping region label pairs
  void getOverlaps (std::list<LabelPair>& overlaps, 
		    LabelImage::Pointer im0, LabelImage::Pointer im1, 
		    bool includeBG0, bool includeBG1);

  /* Get overlapping area of two regions */
  unsigned int getOverlap (Points const& p0, Points const& p1, 
			   LabelImage::Pointer canvas);

  /* Get overlapping region labels of a region */
  void getOverlaps (std::set<Label>& overlaps, Points const& srcRegion, 
		    LabelImage::Pointer refImage, bool includeRefBG);

  // Generate flat region size map
  template <typename TImage> void 
    getSizeMap (std::map<Label, unsigned int>& smap, 
		typename TImage::Pointer im, bool includeBG)
    {
      for (itk::ImageRegionConstIterator<TImage> 
	     it(im, im->GetLargestPossibleRegion()); !it.IsAtEnd(); ++it) {
	std::map<Label, unsigned int>::iterator sit = smap.find(it.Get());
	if (sit != smap.end()) ++(sit->second);
	else smap[it.Get()] = 1;
      }
      if (!includeBG) smap.erase(BGVAL);
    }



  /* Find overlaps on ref image for all regions on src image */
  template <typename TCount> void 
    getOverlaps (std::map<LabelPair, TCount>& overlaps, 
		 LabelImage::Pointer srcImage, 
		 LabelImage::Pointer refImage, 
		 bool includeSrcBG, bool includeRefBG) 
    {
      itk::ImageRegionConstIterator<LabelImage> 
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
		 Points const& srcRegion, LabelImage::Pointer refImage, 
		 bool includeRefBG)
    {
      for (Points::const_iterator pit = srcRegion.begin(); 
	   pit != srcRegion.end(); ++pit) {
	Label ref = getv<LabelImage>(refImage, pit->x, pit->y);
	if (ref != BGVAL || includeRefBG) {
	  typename std::map<Label, TCount>::iterator oit = 
	    overlaps.find(ref);
	  if (oit != overlaps.end()) ++(oit->second);
	  else overlaps[ref] = 1;
	}
      }
    }


  template <typename TImage> typename TImage::Pointer
    dilateImage (typename TImage::Pointer image, PointMap& rmap)
    {
      if (rmap.count(BGVAL) == 0 || rmap[BGVAL].size() == 0) return image;
      Points openSet = rmap[BGVAL];
      rmap.erase(BGVAL);
      while (openSet.size() > 0) {
	Points changeSet;
	std::list<Label> changeToLabels;
	for (Points::iterator pit = openSet.begin(); pit != openSet.end(); 
	     ++pit) {
	  std::vector<Pixel<Label> > n = 
	    getNeighbors<LabelImage>(*pit, image, 4);
	  Label l = BGVAL;
	  unsigned int a = UINT_MAX;
	  for (std::vector<Pixel<Label> >::const_iterator nit = n.begin(); 
	       nit != n.end(); ++nit) {
	    unsigned int thisa = rmap[nit->val].size();
	    if (nit->val != BGVAL && thisa < a) {
	      a = thisa;
	      l = nit->val;
	    }
	  }
	  if (l != BGVAL) {
	    Points::iterator tpit = pit;
	    --pit;
	    changeSet.splice(changeSet.end(), openSet, tpit);
	    changeToLabels.push_back(l);
	  }
	}
	Points::const_iterator pit = changeSet.begin();
	std::list<Label>::const_iterator lit = changeToLabels.begin();
	while (pit != changeSet.end()) {
	  setv<LabelImage>(image, pit->x, pit->y, *lit);
	  rmap[*lit].push_back(*pit);
	  ++pit;
	  ++lit;
	}
      }
      return image;
    }


  template <typename TImage> typename TImage::Pointer
    dilateImage (typename TImage::Pointer image)
    {
      PointMap rmap;
      getPointMap(rmap, image, true);
      return dilateImage(image, rmap);
    }

};

#endif
