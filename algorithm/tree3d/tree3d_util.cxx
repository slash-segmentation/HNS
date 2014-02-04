#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

// Used in pre-merging
Label n3::tree3d::getMinBoundaryNeighbor (BoundaryTable3 const& bt, 
					  Label r0)
{
  Label r1 = BGVAL;
  Float vmin = FLOAT_MAX;
  for (BoundaryTable3::const_iterator bit = bt.begin(); 
       bit != bt.end(); ++bit) {
    Label l0 = bit->first.first, l1 = bit->first.second;
    if (l0 == r0 || l1 == r0) {
      double v = -getSaliency(bit->second);
      if (v < vmin) {
	r1 = (r0 == l0? l1: l0);
	vmin = v;
      }
    }
  }
  return r1;
}



// Do 2 types of merge
// Input label image will be modified
LabelImage3::Pointer n3::tree3d::merge (LabelImage3::Pointer labelImage, 
					FloatImage3::Pointer pbImage, 
					int ath0, int ath1, double pth)
{
  PointMap3 rmap, cmap;
  getPointMap(rmap, labelImage, true);
  PointLabelMap3 lmap;
  getPointNeighbors(lmap, rmap[BGVAL], labelImage, CRCONN3);
  rmap.erase(BGVAL);
  BoundaryTable3 bt;
  getBoundaryTable(bt, lmap, pbImage);
  std::set<Label> aset, pset;
  for (PointMap3::const_iterator it = rmap.begin(); it != rmap.end(); ++it) 
    if (it->second.size() <= ath0) aset.insert(it->first);
    else if (it->second.size() <= ath1) pset.insert(it->first);
  // Type 0 merge 
  while (aset.size() > 0) {
    std::set<Label>::iterator sit = aset.begin();
    Label rfrom = *sit;
    PointMap3::const_iterator fit = rmap.find(rfrom);
    if (fit != rmap.end() && fit->second.size() <= ath0) {
      Label rto = getMinBoundaryNeighbor(bt, rfrom);
      if (rto != BGVAL) {
	updateBoundaryTable(bt, lmap, rfrom, rto);
	merge(rmap, lmap, rfrom, rto);
	PointMap3::const_iterator tit = rmap.find(rto);
	if (tit->second.size() > ath1) {
	  aset.erase(rto);
	  pset.erase(rto);
	}
	else if (tit->second.size() > ath0) {
	  aset.erase(rto);
	  pset.insert(rto);
	}
	else aset.insert(rto);
      }
    }
    aset.erase(sit);
  }
  // Type 1 merge
  while (pset.size() > 0) {
    std::set<Label>::iterator sit = pset.begin();
    Label rfrom = *sit;
    PointMap3::const_iterator fit = rmap.find(rfrom);
    if (fit != rmap.end() && fit->second.size() <= ath1) {
      std::list<Float> vals;
      getvs<FloatImage3>(vals, pbImage, fit->second);
      Float v = getMean(vals);
      if (v >= pth) {
	Label rto = getMinBoundaryNeighbor(bt, rfrom);
	if (rto != BGVAL) {
	  updateBoundaryTable(bt, lmap, rfrom, rto);
	  merge(rmap, lmap, rfrom, rto);
	  PointMap3::const_iterator tit = rmap.find(rto);
	  if (tit != rmap.end()) 
	    if (tit->second.size() > ath1) pset.erase(rto);
	    else pset.insert(rto);
	}
      }
    }
    pset.erase(sit);
  }
  labelImage->FillBuffer(BGVAL);
  for (PointMap3::const_iterator it = rmap.begin(); it != rmap.end(); 
       ++it) setvs<LabelImage3>(labelImage, it->second, it->first);
  return labelImage;
}



// -median
Float n3::tree3d::getSaliency (std::list<fPixel3> const& pixels)
{
  int len = pixels.size();
  std::list<fPixel3>::const_iterator it = pixels.begin();
  if (len % 2 != 0) {
    std::advance(it, len / 2);
    return -(it->val);
  }
  else {
    std::advance(it, len / 2);
    Float r = it->val;
    --it;
    return -(it->val + r) / 2.0;
  }
}



// In key <r0, r1> keep r0 < r1
void n3::tree3d::getBoundaryTable (BoundaryTable3& bt,
				   PointLabelMap3 const& lmap, 
				   FloatImage3::Pointer valImage)
{
  for (PointLabelMap3::const_iterator pit = lmap.begin(); 
       pit != lmap.end(); ++pit) 
    for (LabelSet::const_iterator lit0 = pit->second.begin(); 
	 lit0 != pit->second.end(); ++lit0) {
      LabelSet::const_iterator lit1 = lit0; ++lit1;
      while (lit1 != pit->second.end()) {
	LabelPair key = *lit0 < *lit1? std::make_pair(*lit0, *lit1): 
	  std::make_pair(*lit1, *lit0);
	bt[key].push_back(fPixel3(pit->first, 
				  getv<FloatImage3>(valImage, 
						    pit->first)));
	++lit1;
      }
    }
  // Remove possible merges between two regions without any contour points
  // That are exclusively between the two regions
  std::list<BoundaryTable3::iterator> rms;
  for (BoundaryTable3::iterator bit = bt.begin(); bit != bt.end(); ++bit) {
    bool isExclusivePointFound = false;
    for (std::list<fPixel3>::const_iterator pit = bit->second.begin(); 
	 pit != bit->second.end(); ++pit) {
      PointLabelMap3::const_iterator lit = lmap.find(*pit);
      if (lit->second.size() == 2 && 
	  lit->second.count(bit->first.first) > 0 && 
	  lit->second.count(bit->first.second) > 0) {
	isExclusivePointFound = true; 
	break;
      }
    }
    if (!isExclusivePointFound) rms.push_back(bit);
  }
  remove(bt, rms);
  // Keep boundary pixels in order
  for (BoundaryTable3::iterator tit = bt.begin(); tit != bt.end(); ++tit)
    tit->second.sort();
}



// In key <r0, r1> keep r0 < r1
void n3::tree3d::getBoundaryTable (BoundaryTable3& bt, MergeQueue& mq, 
				   PointLabelMap3 const& lmap, 
				   FloatImage3::Pointer valImage)
{
  getBoundaryTable(bt, lmap, valImage);
  for (BoundaryTable3::const_iterator tit = bt.begin(); 
       tit != bt.end(); ++tit)
    mq.push(fMerge(tit->first.first, tit->first.second, 0, 
		   getSaliency(tit->second)));
}



// Assume r01 is greater than any existing region label
// Use mq = NULL to skip updating it
// Use removed = NULL to skip collecting removed label pairs
void n3::tree3d::updateBoundaryTable (BoundaryTable3& bt, MergeQueue* mq, 
				      std::set<LabelPair>* removed, 
				      Label r0, Label r1, Label r01)
{
  if (r0 > r1) std::swap(r0, r1); // Keep r0 < r1
  if (removed != NULL) removed->insert(std::make_pair(r0, r1));
  bt.erase(std::make_pair(r0, r1));
  std::list<BoundaryTable3::iterator> erm; // Entries to remove
  std::map<Label, std::list<fPixel3> > neighbors;
  for (BoundaryTable3::iterator it = bt.begin(); it != bt.end(); ++it) 
    if (it->first.first == r0 || it->first.first == r1) {
      if (removed != NULL) removed->insert(it->first);
      erm.push_back(it);
      merge(neighbors[it->first.second], it->second, false);
    }
    else if (it->first.second == r0 || it->first.second == r1) {
      if (removed != NULL) removed->insert(it->first);
      erm.push_back(it);
      merge(neighbors[it->first.first], it->second, false);
    }
  remove(bt, erm); // Remove all entries related to r0/r1
  for (std::map<Label, std::list<fPixel3> >::iterator nit = 
	 neighbors.begin(); nit != neighbors.end(); ++nit) {
    LabelPair lp = std::make_pair(nit->first, r01);
    std::list<fPixel3>* pp = &(bt[lp]);
    splice(*pp, nit->second, true);
    if (mq != NULL) 
      mq->push(fMerge(lp.first, lp.second, 0, getSaliency(*pp)));
  }
}



// // Assume r01 is greater than any existing region label
// // Use mq = NULL to skip updating it
// // Use removed = NULL to skip collecting removed label pairs
// void n3::tree3d::updateBoundaryTable2 (BoundaryTable3& bt, MergeQueue* mq, 
// 				       std::set<LabelPair>* removed, 
// 				       Label r0, Label r1, Label r01)
// {
//   if (r0 > r1) std::swap(r0, r1); // Keep r0 < r1
//   if (removed != NULL) removed->insert(std::make_pair(r0, r1));
//   bt.erase(std::make_pair(r0, r1));
//   std::list<BoundaryTable3::iterator> erm; // Entries to remove
//   std::map<Label, std::list<fPixel3>*> neighbors;
//   for (BoundaryTable3::iterator it = bt.begin(); it != bt.end(); ++it) 
//     if (it->first.first == r0 || it->first.first == r1) {
//       if (removed != NULL) removed->insert(it->first);
//       erm.push_back(it);
//       neighbors[it->first.second] = &(it->second);
//     }
//     else if (it->first.second == r0 || it->first.second == r1) {
//       if (removed != NULL) removed->insert(it->first);
//       erm.push_back(it);
//       neighbors[it->first.first] = &(it->second);
//     }
//   for (std::map<Label, std::list<fPixel3> >::iterator nit = 
// 	 neighbors.begin(); nit != neighbors.end(); ++nit) {
//     LabelPair lp = std::make_pair(nit->first, r01);
//     std::list<fPixel3>* pp = &(bt[lp]);
//     merge(*pp, nit->second, true);
//     if (mq != NULL) 
//       mq->push(fMerge(lp.first, lp.second, 0, getSaliency(*pp)));
//   }
//   remove(bt, erm); // Remove all entries related to r0/r1
// }


// Merge from rfrom to rto
void n3::tree3d::updateBoundaryTable (BoundaryTable3& bt, 
				      PointLabelMap3 const& lmap, 
				      Label rfrom, Label rto)
{
  if (rfrom > rto) bt.erase(std::make_pair(rto, rfrom));
  else bt.erase(std::make_pair(rfrom, rto));
  std::list<BoundaryTable3::iterator> erm;
  std::map<Label, std::list<fPixel3> > neighbors;
  for (BoundaryTable3::iterator it = bt.begin(); it != bt.end(); ++it) {
    Label l0 = it->first.first, l1 = it->first.second;
    if (l0 == rfrom) {
      erm.push_back(it);
      merge(neighbors[l1], it->second, false);
    }
    else if (l1 == rfrom) {
      erm.push_back(it);
      merge(neighbors[l0], it->second, false);
    }
  }
  remove(bt, erm);
  for (std::map<Label, std::list<fPixel3> >::iterator it = 
	 neighbors.begin(); it != neighbors.end(); ++it) {
    LabelPair key = (it->first < rto? std::make_pair(it->first, rto): 
		     std::make_pair(rto, it->first));
    merge(bt[key], it->second, true);
  }
}



Label n3::tree3d::getMaxLabel (PointLabelMap3 const& lmap)
{
  Label ret = 0;
  for (PointLabelMap3::const_iterator lit = lmap.begin(); 
       lit != lmap.end(); ++lit)
    for (LabelSet::const_iterator sit = lit->second.begin(); 
	 sit != lit->second.end(); ++sit) 
      if (*sit > ret) ret = *sit;
  return ret;
}



// bp: all 0 pixel points
void n3::tree3d::getMerges (std::list<fMerge>& merges, Points3 const& bp,
			    LabelImage3::Pointer labelImage, 
			    FloatImage3::Pointer valImage)
{
  // // For debug
  // time_t local = time(0);
  // // ~ For debug
  PointLabelMap3 lmap;
  getPointNeighbors(lmap, bp, labelImage, CRCONN3);
  // // For debug
  // std::cerr << "generating lmap took " 
  // 	    << difftime(time(0), local) / 60.0 << std::endl;
  // local = time(0);
  // // ~ For debug
  BoundaryTable3 bt;
  MergeQueue mq;
  getBoundaryTable(bt, mq, lmap, valImage);
  // // For debug
  // std::cerr << "generating bt took " 
  // 	    << difftime(time(0), local) / 60.0 << std::endl;
  // local = time(0);
  // // ~ For debug
  std::set<LabelPair> removed;
  Label r01 = getMaxLabel(lmap) + 1;
  // // For debug
  // local = time(0);
  // // ~ For debug
  while (mq.size() > 0) {
    fMerge m = mq.top();
    mq.pop();
    std::set<LabelPair>::iterator rit = 
      removed.find(std::make_pair(m.from0, m.from1));
    if (rit == removed.end()) {
      Label r0 = m.from0, r1 = m.from1;
      updateBoundaryTable(bt, &mq, &removed, r0, r1, r01);
      // Saliency = -median
      // But store median (not real saliency) in a merge
      merges.push_back(fMerge(r0, r1, r01, -(m.data)));
      merge(NULL, lmap, r0, r1, r01);
      ++r01;
      // // For debug
      // std::cerr << "generated a merge (" << r0 << ", " << r1 << ") took" 
      // 		<< difftime(time(0), local) << std::endl;
      // local = time(0);
      // // ~ For debug
    }
    else removed.erase(rit);
  }
}



void n3::tree3d::getMerges (std::list<fMerge>& merges, 
			    LabelImage3::Pointer labelImage, 
			    FloatImage3::Pointer valImage)
{
  Points3 bp;
  getPoints(bp, labelImage, BGVAL);
  getMerges(merges, bp, labelImage, valImage);
}



void n3::tree3d::getBoundary (Points3& boundary, Points3 const& contour0, 
			      Points3 const& contour1, 
			      LabelImage3::Pointer canvas)
{
  canvas->FillBuffer(0);
  setvs<LabelImage3>(canvas, contour0, 1);
  for (Points3::const_iterator it = contour1.begin(); 
       it != contour1.end(); ++it) 
    if (getv<LabelImage3>(canvas, *it) == 1) boundary.push_back(*it);
}



// Given whether a leaf node touches image borders
// Determine whether all nodes touch image borders
// bmap: label -> image border points
void n3::tree3d::getNodeBorderIndicators (std::vector<bool>& indicators, 
					  fTree const& tree, 
					  PointMap3 const& bmap)
{
  indicators.resize(tree.size(), false);
  for (fTree::const_iterator tit = tree.begin(); tit != tree.end(); ++tit)
    if (tit->child0 < 0 && tit->child1 < 0) {
      if (bmap.count(tit->label) != 0) indicators[tit->self] = true;
    }
    else 
      indicators[tit->self] = (indicators[tit->child0] || 
			       indicators[tit->child1]);
}
