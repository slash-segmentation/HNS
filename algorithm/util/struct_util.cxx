#include "struct_util.h"
using namespace n3;

// Get points with given label
void n3::getPoints (Points& p, LabelImage::Pointer im, Label label)
{
  for (itk::ImageRegionConstIteratorWithIndex<LabelImage> 
	 it(im, im->GetLargestPossibleRegion()); !it.IsAtEnd(); ++it) 
    if (it.Get() == label) p.push_back(i2p(it.GetIndex()));
}



// Get union of two points
void n3::getUnionPoints (Points& p, Points const& p0, Points const& p1, 
			 LabelImage::Pointer canvas)
{
  canvas->FillBuffer(0);
  setvs<LabelImage>(canvas, p0, 1);
  for (Points::const_iterator it = p1.begin(); it != p1.end(); ++it)
    if (getv<LabelImage>(canvas, it->x, it->y) == 1) 
      p.push_back(*it);
}



// Generate flat region point map
void n3::getPointMap (PointMap& rmap, LabelImage::Pointer im, 
		      bool includeBG)
{
  for (itk::ImageRegionConstIteratorWithIndex<LabelImage> 
	 it(im, im->GetLargestPossibleRegion()); !it.IsAtEnd(); ++it)
    rmap[it.Get()].push_back(i2p(it.GetIndex()));
  if (!includeBG) rmap.erase(BGVAL);
}



// Return rmap; generate if none
// Read in label image if none
PointMap* n3::getPointMap 
(std::map<int, PointMap>& rmaps, 
 std::map<int, LabelImage::Pointer>& images, 
 std::map<int, const char*> const& imageNames, int slice)
{
  std::map<int, PointMap>::iterator rit = rmaps.find(slice);
  if (rit != rmaps.end()) return &(rit->second);
  LabelImage::Pointer image = 
    getImage<LabelImage>(images, imageNames, slice);
  PointMap* ret = &rmaps[slice];
  getPointMap(*ret, image, false);
  return ret;
}




// Get neighbhor region label for each contour point
void n3::getPointNeighbors (PointLabelMap& lmap, Points const& points, 
			    LabelImage::Pointer im, int connect) 
{
  typedef std::vector<Pixel<Label> > LabelPixels;
  for (Points::const_iterator pit = points.begin(); 
       pit != points.end(); ++pit) {
    LabelPixels n = getNeighbors<LabelImage>(*pit, im, connect);
    for (LabelPixels::const_iterator nit = n.begin(); 
	 nit != n.end(); ++nit) 
      if (nit->val != BGVAL) lmap[*pit].insert(nit->val);
  }
}



// // Get contour point for each region label
// void n3::getNeighborPoints (PointMap& cmap, PointLabelMap const& lmap)
// {
//   for (PointLabelMap::const_iterator pit = lmap.begin(); 
//        pit != lmap.end(); ++pit)
//     for (LabelSet::const_iterator nit = pit->second.begin(); 
// 	 nit != pit->second.end(); ++nit) cmap[*nit].push_back(pit->first);
// }

//------------------------------------------------------------------

// Set sort to true for faster merge
void n3::getNeighborPoints (PointMap& cmap, PointLabelMap const& lmap, 
			    bool sort)
{
  for (PointLabelMap::const_iterator pit = lmap.begin(); 
       pit != lmap.end(); ++pit)
    for (LabelSet::const_iterator nit = pit->second.begin(); 
	 nit != pit->second.end(); ++nit) cmap[*nit].push_back(pit->first);
  if (sort)
    for (PointMap::iterator cit = cmap.begin(); cit != cmap.end(); ++cit) 
      cit->second.sort();
}



// Generate flat rmap and cmap
// Set sort to true for faster merge
void n3::getPointMap (PointMap& rmap, PointLabelMap& lmap, 
		      PointMap& cmap, LabelImage::Pointer im, 
		      int connect, bool includeBG, bool sort)
{
  getPointMap(rmap, im, true);
  PointMap::iterator bgit = rmap.find(BGVAL);
  getPointNeighbors(lmap, bgit->second, im, connect);
  if (!includeBG) rmap.erase(bgit);
  getNeighborPoints(cmap, lmap, sort);
}



// c2rp: contour points becoming region points due to merging
void n3::merge (Points* c2rp, PointLabelMap& lmap, Label r0, 
		Label r1, Label r01)
{
  std::list<PointLabelMap::iterator> irm; // Iterators to remove
  for (PointLabelMap::iterator it = lmap.begin(); 
       it != lmap.end(); ++it) {
    LabelSet::iterator it0 = it->second.find(r0);
    LabelSet::iterator it1 = it->second.find(r1);
    // Find points exclusively between r0 and r1
    if (it->second.size() == 2 && it0 != it->second.end() && 
	it1 != it->second.end()) {
      if (c2rp != NULL) c2rp->push_back(it->first);
      irm.push_back(it);
    }
    // Remove all other r0/r1 labels and insert r01
    else if (it0 != it->second.end() || it1 != it->second.end()) {
      it->second.insert(r01);
      if (it0 != it->second.end()) it->second.erase(it0);
      if (it1 != it->second.end()) it->second.erase(it1);
    }
  }
  for (std::list<PointLabelMap::iterator>::const_iterator it = 
	 irm.begin(); it != irm.end(); ++it) 
    lmap.erase(*it); // Remove from lmap
}



// Set sort to true for faster merge; cmap must be sorted
void n3::merge (Points* c2rp, PointMap& cmap, PointLabelMap& lmap, 
		Label r0, Label r1, Label r01, bool sort, bool keepSrc, 
		LabelImage::Pointer canvas)
{
  // Append contour point map and change boundary point assignment map
  Points* cp01 = &(cmap[r01]);
  PointMap::iterator cp0 = cmap.find(r0);
  PointMap::iterator cp1 = cmap.find(r1);
  if (keepSrc) {
    if (sort) {
      if (cp0->second.size() < cp1->second.size()) std::swap(cp0, cp1);
      *cp01 = cp0->second;
      Points cc1 = cp1->second;
      merge(*cp01, cc1, true);
    }
    else combine(*cp01, cp0->second, cp1->second, true);
  }
  else {
    splice(*cp01, cp0->second, false);
    if (sort) merge(*cp01, cp1->second, true);
    else splice(*cp01, cp1->second, true);
    cmap.erase(cp0);
    cmap.erase(cp1);
  }
  merge(c2rp, lmap, r0, r1, r01);
  // remove(*cp01, *c2rp, false); // Remove from contour point map: SLOW??
  // remove(*cp01, *c2rp, canvas);	// Remove from contour point map
  remove(*cp01, *c2rp, false, true);
}



// Set sort to true for faster contour merge; cmap must be sorted
void n3::merge (PointMap& rmap, PointMap& cmap, PointLabelMap& lmap, 
		Label r0, Label r1, Label r01, bool sort, bool keepSrc, 
		LabelImage::Pointer canvas)
{
  Points c2rp;
  merge(&c2rp, cmap, lmap, r0, r1, r01, sort, keepSrc, canvas);
  // Append region point
  Points* rp = &(rmap[r01]);
  PointMap::iterator rit0 = rmap.find(r0); 
  PointMap::iterator rit1 = rmap.find(r1);
  if (keepSrc) combine(*rp, rit0->second, rit1->second, false);
  else {
    splice(*rp, rit0->second, false);
    splice(*rp, rit1->second, false);
    rmap.erase(rit0);
    rmap.erase(rit1);
  }
  // Append contour points to inside points
  append(*rp, c2rp, false);
}

//------------------------------------------------------------------


// // Only generate merged region and contour points
// // Do not modify any other structure
// void n3::merge (Points& r01, Points& c01, Label l0, Label l1, 
// 		Points const& r0, Points const& r1, Points const& c0, 
// 		Points const& c1, PointLabelMap const& lmap)
// {
//   combine(c01, c0, c1, false);
//   Points c2rp;
//   for (PointLabelMap::const_iterator it = lmap.begin(); 
//        it != lmap.end(); ++it) {
//     LabelSet::const_iterator it0 = it->second.find(l0);
//     LabelSet::const_iterator it1 = it->second.find(l1);
//     if (it->second.size() == 2 && it0 != it->second.end() && 
// 	it1 != it->second.end()) c2rp.push_back(it->first);
//   }
//   remove(c01, c2rp, false, false);
//   combine(r01, r0, r1, false);
//   append(r01, c2rp, false);
// }



// // c2rp: contour points to remove (becoming region points)
// void n3::merge (Points& c2rp, PointMap& cmap, PointLabelMap& lmap, 
// 		Label r0, Label r1, Label r01)
// {
//   // Append contour point map and change boundary point assignment map
//   Points* cp01 = &(cmap[r01]);
//   combine(*cp01, cmap[r0], cmap[r1], true);
//   std::list<PointLabelMap::iterator> irm; // Iterators to remove
//   for (PointLabelMap::iterator it = lmap.begin(); 
//        it != lmap.end(); ++it) {
//     LabelSet::iterator it0 = it->second.find(r0);
//     LabelSet::iterator it1 = it->second.find(r1);
//     // Find points exclusively between r0 and r1
//     if (it->second.size() == 2 && it0 != it->second.end() && 
// 	it1 != it->second.end()) {
//       c2rp.push_back(it->first);
//       irm.push_back(it);
//     }
//     // Remove all other r0/r1 labels and insert r01
//     else if (it0 != it->second.end() || it1 != it->second.end()) {
//       it->second.insert(r01);
//       if (it0 != it->second.end()) it->second.erase(it0);
//       if (it1 != it->second.end()) it->second.erase(it1);
//     }
//   }
//   remove(*cp01, c2rp, false, false); // Remove from contour point map
//   for (std::list<PointLabelMap::iterator>::const_iterator it = 
// 	 irm.begin(); it != irm.end(); ++it) 
//     lmap.erase(*it); // Remove from lmap
// }



// // Merge r0 and r1 to r01
// // rpmap: region point map (to be appended by inside points of r01)
// // rpmap: also to be appended by boundary points between r0/r1
// // bpmap: contour point map (to be appended by contour points of r01)
// // bplmap: boundary point assignment map (to be changed)
// // bplmap: remove all r0/r1 labels, insert r01 labels
// void n3::merge (PointMap& rmap, PointMap& cmap, PointLabelMap& lmap, 
// 		Label r0, Label r1, Label r01)
// {
//   Points c2rp;
//   merge(c2rp, cmap, lmap, r0, r1, r01);
//   // Append region point
//   Points* rp = &(rmap[r01]);
//   combine(*rp, rmap[r0], rmap[r1], false);
//   // Append contour points to inside points
//   append(*rp, c2rp, false);
// }



// Get overlapping region label pairs
void n3::getOverlaps (std::list<LabelPair>& overlaps, 
		      LabelImage::Pointer im0, LabelImage::Pointer im1, 
		      bool includeBG0, bool includeBG1)
{
  itk::ImageRegionConstIterator<LabelImage> 
    it0(im0, im0->GetLargestPossibleRegion()), 
    it1(im1, im1->GetLargestPossibleRegion());
  Label l0 = it0.Get(), l1 = it1.Get();
  LabelPair prev = std::make_pair(l0, l1); // Previous pair
  if ((l0 != BGVAL || includeBG0) && (l1 != BGVAL || includeBG1))
    overlaps.push_back(prev);
  while (!it0.IsAtEnd()) {
    l0 = it0.Get();
    l1 = it1.Get();
    if (l0 != prev.first || l1 != prev.second) {
      if ((l0 != BGVAL || includeBG0) && (l1 != BGVAL || includeBG1))
  	overlaps.push_back(std::make_pair(l0, l1));
      prev.first = l0;
      prev.second = l1;
    }
    ++it0;
    ++it1;
  }
  overlaps.sort();
  overlaps.unique();
}



unsigned int n3::getOverlap (Points const& p0, Points const& p1, 
			     LabelImage::Pointer canvas)
{
  canvas->FillBuffer(0);
  setvs<LabelImage>(canvas, p0, 1);
  unsigned int ret = 0;
  for (Points::const_iterator pit = p1.begin(); pit != p1.end(); ++pit)
    if (getv<LabelImage>(canvas, pit->x, pit->y) == 1) ++ret;
  return ret;
}



void n3::getOverlaps (std::set<Label>& overlaps, Points const& srcRegion, 
		     LabelImage::Pointer refImage, bool includeRefBG) 
{
  for (Points::const_iterator pit = srcRegion.begin(); 
       pit != srcRegion.end(); ++pit) {
    Label ref = getv<LabelImage>(refImage, pit->x, pit->y);
    if (ref != BGVAL || includeRefBG) overlaps.insert(ref);
  }
}
