#include "struct_util3.h"
using namespace n3;

void n3::getPoints (Points3& p, LabelImage3::Pointer im, Label label)
{
  for (itk::ImageRegionConstIteratorWithIndex<LabelImage3> 
	 it(im, im->GetLargestPossibleRegion()); !it.IsAtEnd(); ++it) 
    if (it.Get() == label) p.push_back(i2p(it.GetIndex()));
}



void n3::getPointMap (PointMap3& rmap, LabelImage3::Pointer im, 
		      bool includeBG)
{
  for (itk::ImageRegionConstIteratorWithIndex<LabelImage3> 
	 it(im, im->GetLargestPossibleRegion()); !it.IsAtEnd(); ++it) 
    rmap[it.Get()].push_back(i2p(it.GetIndex()));
  if (!includeBG) rmap.erase(BGVAL);
}



void n3::getPointNeighbors (PointLabelMap3& lmap, Points3 const& points, 
			    LabelImage3::Pointer im, int connect) 
{
  typedef std::vector<Pixel3<Label> > LabelPixels;
  for (Points3::const_iterator pit = points.begin(); 
       pit != points.end(); ++pit) {
    LabelPixels n = getNeighbors<LabelImage3>(*pit, im, connect);
    for (LabelPixels::const_iterator nit = n.begin(); 
	 nit != n.end(); ++nit) 
      if (nit->val != BGVAL) lmap[*pit].insert(nit->val);
  }
}



// Set sort to true for faster merge
void n3::getNeighborPoints (PointMap3& cmap, PointLabelMap3 const& lmap, 
			    bool sort)
{
  for (PointLabelMap3::const_iterator pit = lmap.begin(); 
       pit != lmap.end(); ++pit)
    for (LabelSet::const_iterator nit = pit->second.begin(); 
	 nit != pit->second.end(); ++nit) cmap[*nit].push_back(pit->first);
  if (sort)
    for (PointMap3::iterator cit = cmap.begin(); cit != cmap.end(); ++cit) 
      cit->second.sort();
}



// Generate flat rmap and cmap
// Set sort to true for faster merge
void n3::getPointMap (PointMap3& rmap, PointLabelMap3& lmap, 
		      PointMap3& cmap, LabelImage3::Pointer im, 
		      int connect, bool includeBG, bool sort)
{
  getPointMap(rmap, im, true);
  PointMap3::iterator bgit = rmap.find(BGVAL);
  getPointNeighbors(lmap, bgit->second, im, connect);
  if (!includeBG) rmap.erase(bgit);
  getNeighborPoints(cmap, lmap, sort);
}



// c2rp: contour points becoming region points due to merging
// Since lmap's keys are sorted, c2rp is sorted
void n3::merge (Points3* c2rp, PointLabelMap3& lmap, Label r0, 
		Label r1, Label r01)
{
  std::list<PointLabelMap3::iterator> irm; // Iterators to remove
  for (PointLabelMap3::iterator it = lmap.begin(); 
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
  for (std::list<PointLabelMap3::iterator>::const_iterator it = 
	 irm.begin(); it != irm.end(); ++it) 
    lmap.erase(*it); // Remove from lmap
}



// Set sort to true for faster merge; cmap must be sorted
void n3::merge (Points3* c2rp, PointMap3& cmap, PointLabelMap3& lmap, 
		Label r0, Label r1, Label r01, bool sort, bool keepSrc)
{
  // Append contour point map and change boundary point assignment map
  Points3* cp01 = &(cmap[r01]);
  PointMap3::iterator cp0 = cmap.find(r0);
  PointMap3::iterator cp1 = cmap.find(r1);
  if (keepSrc) {
    if (sort) {
      if (cp0->second.size() < cp1->second.size()) std::swap(cp0, cp1);
      *cp01 = cp0->second;
      Points3 cc1 = cp1->second;
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
  remove(*cp01, *c2rp, false, sort);
}



// Set sort to true for faster contour merge; cmap must be sorted
void n3::merge (PointMap3& rmap, PointMap3& cmap, PointLabelMap3& lmap, 
		Label r0, Label r1, Label r01, bool sort, bool keepSrc)
{
  Points3 c2rp;
  merge(&c2rp, cmap, lmap, r0, r1, r01, sort, keepSrc);
  // Append region point
  Points3* rp = &(rmap[r01]);
  PointMap3::iterator rit0 = rmap.find(r0); 
  PointMap3::iterator rit1 = rmap.find(r1);
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



// Set sort to true for faster contour merge; cmap must be sorted
// Also combine border points in bmap if any
void n3::merge (PointMap3& rmap, PointMap3& cmap, PointLabelMap3& lmap, 
		PointMap3& bmap, Label r0, Label r1, Label r01, 
		bool sort, bool keepSrc)
{
  merge(rmap, cmap, lmap, r0, r1, r01, sort, keepSrc);
  PointMap3::iterator bit0 = bmap.find(r0), bit1 = bmap.find(r1);
  if (bit0 != bmap.end() || bit1 != bmap.end()) {
    PointMap3::iterator bit = 
      bmap.insert(bmap.end(), std::make_pair(r01, Points3()));
    if (bit0 != bmap.end() && bit1 != bmap.end()) {
      if (keepSrc) 
	combine(bit->second, bit0->second, bit1->second, false, sort);
      else {
	if (sort) {
	  merge(bit->second, bit0->second, false);
	  merge(bit->second, bit1->second, false);
	}
	else {
	  splice(bit->second, bit0->second, false);
	  splice(bit->second, bit1->second, false);	  
	}
	bmap.erase(bit0);
	bmap.erase(bit1);
      }
    }
    else if (bit0 != bmap.end()) {
      if (keepSrc) bit->second = bit0->second;
      else {
	splice(bit->second, bit0->second, false);
	bmap.erase(bit0);
      }
    }
    else { // bit1 != bmap.end()
      if (keepSrc) bit->second = bit1->second;
      else {
	splice(bit->second, bit1->second, false);
	bmap.erase(bit1);
      }
    }
  }
}



// Assume merging rfrom to rto
// Used in pre-merging
void n3::merge (PointMap3& rmap, PointLabelMap3& lmap, Label rfrom, 
		Label rto)
{
  Points3 c2rp;
  std::list<PointLabelMap3::iterator> irm;
  for (PointLabelMap3::iterator it = lmap.begin(); it != lmap.end(); 
       ++it) {
    LabelSet::iterator fit = it->second.find(rfrom);
    LabelSet::iterator tit = it->second.find(rto);
    if (it->second.size() == 2 && fit != it->second.end() && 
	tit != it->second.end()) {
      c2rp.push_back(it->first);
      irm.push_back(it);
    }
    else if (fit != it->second.end()) {
      if (tit == it->second.end()) it->second.insert(rto);
      it->second.erase(fit);
    }
  }
  for (std::list<PointLabelMap3::iterator>::const_iterator it = 
	 irm.begin(); it != irm.end(); ++it) lmap.erase(*it);
  splice(rmap[rto], rmap[rfrom], false);
  splice(rmap[rto], c2rp, false);
  rmap.erase(rfrom);
}



// Get overlapping region label pairs
void n3::getOverlaps (std::list<LabelPair>& overlaps, 
		      LabelImage3::Pointer im0, LabelImage3::Pointer im1, 
		      bool includeBG0, bool includeBG1)
{
  itk::ImageRegionConstIterator<LabelImage3> 
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



unsigned int n3::getOverlap (Points3 const& p0, Points3 const& p1, 
			     LabelImage3::Pointer canvas)
{
  canvas->FillBuffer(0);
  setvs<LabelImage3>(canvas, p0, 1);
  unsigned int ret = 0;
  for (Points3::const_iterator pit = p1.begin(); pit != p1.end(); ++pit)
    if (getv<LabelImage3>(canvas, pit->x, pit->y, pit->z) == 1) ++ret;
  return ret;
}



void n3::getOverlaps (std::set<Label>& overlaps, Points3 const& srcRegion, 
		      LabelImage3::Pointer refImage, bool includeRefBG) 
{
  for (Points3::const_iterator pit = srcRegion.begin(); 
       pit != srcRegion.end(); ++pit) {
    Label ref = getv<LabelImage3>(refImage, pit->x, pit->y, pit->z);
    if (ref != BGVAL || includeRefBG) overlaps.insert(ref);
  }
}



// // Collect all points on image borders
// void n3::getBorderPoints (PointMap3& bmap, LabelImage3::Pointer image, 
// 			  bool includeBG)
// {
//   int w = getw<LabelImage3>(image), h = geth<LabelImage3>(image), 
//     d = getd<LabelImage3>(image);
//   int xmax = w - 1, ymax = h - 1, zmax = d - 1;
//   // z = 0 and z = d - 1 and 8 edges and 8 vertices
//   for (int x = 0; x < w; ++x) 
//     for (int y = 0; y < h; ++y) {
//       Label l = getv<LabelImage3>(image, x, y, 0);
//       if (l != BGVAL || includeBG) bmap[l].push_back(Point3(x, y, 0));
//       l = getv<LabelImage3>(image, x, y, zmax);
//       if (l != BGVAL || includeBG) bmap[l].push_back(Point3(x, y, zmax));
//     }
//   // y = 0 and y = h - 1 and 4 edges
//   for (int x = 0; x < w; ++x)
//     for (int z = 1; z < zmax; ++z) {
//       Label l = getv<LabelImage3>(image, x, 0, z);
//       if (l != BGVAL || includeBG) bmap[l].push_back(Point3(x, 0, z));
//       l = getv<LabelImage3>(image, x, ymax, z);
//       if (l != BGVAL || includeBG) bmap[l].push_back(Point3(x, ymax, z));
//     }
//   // x = 0 and x = w - 1
//   for (int y = 1; y < ymax; ++y)
//     for (int z = 1; z < zmax; ++z) {
//       Label l = getv<LabelImage3>(image, 0, y, z);
//       if (l != BGVAL || includeBG) bmap[l].push_back(Point3(0, y, z));
//       l = getv<LabelImage3>(image, xmax, y, z);
//       if (l != BGVAL || includeBG) bmap[l].push_back(Point3(xmax, y, z));
//     }
// }



// Collect all points on image borders
// Output points are supposed to be in x-y-z-ascending order
void n3::getBorderPoints (PointMap3& bmap, LabelImage3::Pointer image, 
			  bool includeBG)
{
  const int w = getw<LabelImage3>(image), h = geth<LabelImage3>(image), 
    d = getd<LabelImage3>(image);
  int x, y, z;
  // x = 0, y = 0: h - 1, z = 0: d - 1
  x = 0;
  for (y = 0; y < h; ++y)
    for (z = 0; z < d; ++z) {
      Label l = getv<LabelImage3>(image, x, y, z);
      if (l != BGVAL || includeBG) bmap[l].push_back(Point3(x, y, z));
    }
  // x = 1: w - 2, y = 0, z = 0: d - 1
  y = 0;
  for (x = 1; x < w - 1; ++x) 
    for (z = 0; z < d; ++z) {
      Label l = getv<LabelImage3>(image, x, y, z);
      if (l != BGVAL || includeBG) bmap[l].push_back(Point3(x, y, z));
    }
  // x = 1: w - 2, y = 1: h - 2, z = 0
  z = 0;
  for (x = 1; x < w - 1; ++x) 
    for (y = 1; y < h - 1; ++y) {
      Label l = getv<LabelImage3>(image, x, y, z);
      if (l != BGVAL || includeBG) bmap[l].push_back(Point3(x, y, z));
    }
  // x = 1: w - 2, y = 1: h - 2, z = d - 1
  z = d - 1;
  for (x = 1; x < w - 1; ++x) 
    for (y = 1; y < h - 1; ++y) {
      Label l = getv<LabelImage3>(image, x, y, z);
      if (l != BGVAL || includeBG) bmap[l].push_back(Point3(x, y, z));
    }
  // x = 1: w - 2, y = h - 1, z = 0: d - 1
  y = h - 1;
  for (x = 1; x < w - 1; ++x) 
    for (z = 0; z < d; ++z) {
      Label l = getv<LabelImage3>(image, x, y, z);
      if (l != BGVAL || includeBG) bmap[l].push_back(Point3(x, y, z));
    }
  // x = w - 1, y = 0: h - 1, z = 0: d - 1
  x = w - 1;
  for (y = 0; y < h; ++y) 
    for (z = 0; z < d; ++z) {
      Label l = getv<LabelImage3>(image, x, y, z);
      if (l != BGVAL || includeBG) bmap[l].push_back(Point3(x, y, z));
    }
}
