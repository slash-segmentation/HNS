#ifndef _n3_metric_util_h_
#define _n3_metric_util_h_

#include "struct/bignum.h"
#include "util/struct_util3.h"

namespace n3 {

  /* Given two label images count region pair overlaps */
  void getOverlaps (std::map<LabelPair, BigInt>& overlaps, 
		    std::vector<LabelImage::Pointer> const& resImages, 
		    std::vector<LabelImage::Pointer> const& refImages, 
		    bool includeRefBG = false);

  void getOverlaps (std::map<LabelPair, BigInt>& overlaps, 
		    std::vector<LabelImage3::Pointer> const& resImages, 
		    std::vector<LabelImage3::Pointer> const& refImages, 
		    bool includeRefBG = false);
  
  /* Given a reference image and a set of regions count overlaps */
  /* bg: background points, set to NULL if none */
  void getOverlaps (std::map<LabelPair, BigInt>& overlaps, 
		    std::list<const Points*> const& segs, const Points* bg, 
		    LabelImage::Pointer refImage, 
		    bool includeRefBG = false);

  void getOverlaps (std::map<LabelPair, BigInt>& overlaps, 
		    std::list<const Points3*> const& segs, 
		    const Points3* bg, LabelImage3::Pointer refImage, 
		    bool includeRefBG = false);


  template <typename TCount> void 
    transform (std::map<Label, std::map<Label, TCount> >& tar, 
	       std::map<LabelPair, TCount> const& src)
    {
      for (typename std::map<LabelPair, TCount>::const_iterator sit = 
	     src.begin(); sit != src.end(); ++sit) 
	tar[sit->first.first][sit->first.second] = sit->second;
    }


  /* Just original getOverlap() + transform() */
  template <typename TCount> void 
    getOverlaps (std::map<Label, std::map<Label, TCount> >& overlaps, 
		 LabelImage::Pointer srcImage, 
		 LabelImage::Pointer refImage, bool includeSrcBG, 
		 bool includeRefBG)
    {
      std::map<LabelPair, TCount> toverlaps;
      getOverlaps(toverlaps, srcImage, refImage, includeSrcBG, 
		  includeRefBG);
      transform(overlaps, toverlaps);
    }


  /* Just original getOverlap() + transform() */
  template <typename TCount> void 
    getOverlaps (std::map<Label, std::map<Label, TCount> >& overlaps, 
		 LabelImage3::Pointer srcImage, 
		 LabelImage3::Pointer refImage, bool includeSrcBG, 
		 bool includeRefBG)
    {
      std::map<LabelPair, TCount> toverlaps;
      getOverlaps(toverlaps, srcImage, refImage, includeSrcBG, 
		  includeRefBG);
      transform(overlaps, toverlaps);
    }


  /* Find max in data */
  /* Return false if data is empty, which means m is invalid */
  template <typename TCount> bool
    findMax (std::pair<Label, TCount>& m, 
	     std::map<Label, TCount> const& data)
    {
      if (data.size() == 0) return false;
      typename std::map<Label, TCount>::const_iterator mit = data.begin(); 
      for (typename std::map<Label, TCount>::const_iterator it = 
	     data.begin(); it != data.end(); ++it) 
	if (it->second > mit->second) mit = it;
      m = *mit;
      return true;
    }


  // Find matching regions from ref image for a region on source image 
  // Ignore background pixels on both images
  // Match type can be: 0 (max overlap), 1 (max overlap ratio)
  template <typename TCount> void 
    match (std::map<Label, std::pair<Label, TCount> >& mmap, 
	   LabelImage::Pointer srcImage, LabelImage::Pointer refImage, 
	   int type) 
    {
      if (type == 0) {
	std::map<LabelPair, TCount> overlaps;
	getOverlaps(overlaps, srcImage, refImage, false, false);
	std::map<Label, std::map<Label, TCount> > toverlaps;
	transform(toverlaps, overlaps);
	for (typename std::map<Label, std::map<Label, 
	       TCount> >::const_iterator dit = toverlaps.begin(); 
	     dit != toverlaps.end(); ++dit) {
	  std::pair<Label, TCount> m;
	  if (findMax(m, dit->second)) mmap[dit->first] = m;
	}
      }
      else if (type == 1) {
	std::map<LabelPair, unsigned int> overlaps;
	getOverlaps(overlaps, srcImage, refImage, false, false);
	std::map<Label, unsigned int> ssmap, rsmap;
	getSizeMap<LabelImage>(ssmap, srcImage, false);
	getSizeMap<LabelImage>(rsmap, refImage, false);
	std::map<Label, std::map<Label, TCount> > overlapRatios;
	for (typename std::map<LabelPair, unsigned int>::const_iterator 
	       oit = overlaps.begin(); oit != overlaps.end(); ++oit) {
	  Label src = oit->first.first, ref = oit->first.second;	  
	  overlapRatios[src][ref] = (double)oit->second / 
	    (double)(ssmap[src] + rsmap[ref] - oit->second);
	}
	for (typename std::map<Label, std::map<Label, 
	       TCount> >::const_iterator dit = overlapRatios.begin(); 
	     dit != overlapRatios.end(); ++dit) {
	  std::pair<Label, TCount> m;
	  if (findMax(m, dit->second)) mmap[dit->first] = m;
	}
      }
      else perr("Error: unsupported matching type...");
    }


  /* Find a matching region from ref image for a region on src image  */
  /* Return false if no matching is found, which means m is invalid */
  /* Ignore background pixels on both images */
  /* Match type is max overlap */
  template <typename TCount> bool 
    match (std::pair<Label, TCount>& m, Points const& srcRegion, 
	   LabelImage::Pointer refImage)
    {
      std::map<Label, TCount> overlaps;
      getOverlaps(overlaps, srcRegion, refImage, false);
      if (!findMax(m, overlaps)) return false;
      return true;
    }


  /* Find a matching region from ref image for a region on src image  */
  /* Return false if no matching is found, which means m is invalid */
  /* Ignore background pixels on both images */
  /* Match type is max overlap ratio */
  bool match (std::pair<Label, double>& m, Points const& srcRegion, 
	      int srcSize, LabelImage::Pointer refImage, 
	      std::map<Label, unsigned int> const& refSizeMap);

};

#endif
