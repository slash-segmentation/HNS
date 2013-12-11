#include "tree3d_util.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

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
  for (BoundaryTable3::iterator tit = bt.begin(); tit != bt.end(); ++tit)
    mq.push(fMerge(tit->first.first, tit->first.second, 0, 
		   getSaliency(tit->second)));
}



// Assume r01 is greater than any existing region label
void n3::tree3d::updateBoundaryTable (BoundaryTable3& bt, MergeQueue& mq, 
				      std::set<LabelPair>& removed, 
				      Label r0, Label r1, Label r01)
{
  if (r0 > r1) std::swap(r0, r1); // Keep r0 < r1
  removed.insert(std::make_pair(r0, r1));
  bt.erase(std::make_pair(r0, r1));
  std::list<BoundaryTable3::iterator> erm; // Entries to remove
  std::map<Label, std::list<fPixel3> > neighbors;
  for (BoundaryTable3::iterator it = bt.begin(); it != bt.end(); ++it) 
    if (it->first.first == r0 || it->first.first == r1) {
      removed.insert(it->first);
      erm.push_back(it);
      merge(neighbors[it->first.second], it->second, false);
    }
    else if (it->first.second == r0 || it->first.second == r1) {
      removed.insert(it->first);
      erm.push_back(it);
      merge(neighbors[it->first.first], it->second, false);
    }
  remove(bt, erm); // Remove all entries related to r0/r1
  for (std::map<Label, std::list<fPixel3> >::iterator nit = 
	 neighbors.begin(); nit != neighbors.end(); ++nit) {
    LabelPair lp = std::make_pair(nit->first, r01);
    std::list<fPixel3>* pp = &(bt[lp]);
    merge(*pp, nit->second, true);
    mq.push(fMerge(lp.first, lp.second, 0, getSaliency(*pp)));
  }
}



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



// // bp: all 0 pixel points
// void n3::tree3d::getMerges (std::list<fMerge>& merges, 
// 			    PointMap3& cmap, Points3 const& bp, 
// 			    LabelImage3::Pointer labelImage, 
// 			    FloatImage3::Pointer valImage)
// {
//   // For debug
//   time_t start = time(0);
//   time_t local = time(0);
//   // ~ For debug
//   PointLabelMap3 lmap;
//   getPointNeighbors(lmap, bp, labelImage, CRCONN3);
//   getNeighborPoints(cmap, lmap);
//   // For debug
//   std::cerr << "generating lmap and cmap took " 
// 	    << difftime(time(0), local) / 60.0 << std::endl;
//   local = time(0);
//   // ~ For debug
//   BoundaryTable3 bt;
//   MergeQueue mq;
//   getBoundaryTable(bt, mq, lmap, valImage);
//   // For debug
//   std::cerr << "generating bt took " 
// 	    << difftime(time(0), local) / 60.0 << std::endl;
//   local = time(0);
//   // ~ For debug
//   std::set<LabelPair> removed;
//   Label r01 = cmap.rbegin()->first; ++r01;
//   while (mq.size() > 0) {
//     fMerge m = mq.top();
//     mq.pop();
//     std::set<LabelPair>::iterator rit = 
//       removed.find(std::make_pair(m.from0, m.from1));
//     if (rit == removed.end()) {
//       Label r0 = m.from0, r1 = m.from1;
//       updateBoundaryTable(bt, mq, removed, r0, r1, r01);
//       // Saliency = -median; but store median (not saliency) in a merge
//       merges.push_back(fMerge(r0, r1, r01, -(m.data)));
//       Points3 c2rp;
//       merge(c2rp, cmap, lmap, r0, r1, r01);
//       ++r01;
//       // For debug
//       std::cerr << "generated a merge (" << r0 << ", " << r1 << ")" 
// 		<< std::endl;
//       // ~ For debug
//     }
//     else removed.erase(rit);
//   }
// }


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
  // For debug
  time_t local = time(0);
  // ~ For debug
  PointLabelMap3 lmap;
  getPointNeighbors(lmap, bp, labelImage, CRCONN3);
  // For debug
  std::cerr << "generating lmap took " 
	    << difftime(time(0), local) / 60.0 << std::endl;
  local = time(0);
  // ~ For debug
  BoundaryTable3 bt;
  MergeQueue mq;
  getBoundaryTable(bt, mq, lmap, valImage);
  // For debug
  std::cerr << "generating bt took " 
	    << difftime(time(0), local) / 60.0 << std::endl;
  local = time(0);
  // ~ For debug
  std::set<LabelPair> removed;
  Label r01 = getMaxLabel(lmap) + 1;
  // For debug
  local = time(0);
  // ~ For debug
  while (mq.size() > 0) {
    fMerge m = mq.top();
    mq.pop();
    std::set<LabelPair>::iterator rit = 
      removed.find(std::make_pair(m.from0, m.from1));
    if (rit == removed.end()) {
      Label r0 = m.from0, r1 = m.from1;
      updateBoundaryTable(bt, mq, removed, r0, r1, r01);
      // Saliency = -median; but store median (not saliency) in a merge
      merges.push_back(fMerge(r0, r1, r01, -(m.data)));
      merge(NULL, lmap, r0, r1, r01);
      ++r01;
      // For debug
      std::cerr << "generated a merge (" << r0 << ", " << r1 << ") took" 
		<< difftime(time(0), local) << std::endl;
      local = time(0);
      // ~ For debug
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
			      LabelImage3::Pointer& canvas)
{
  canvas->FillBuffer(0);
  setvs<LabelImage3>(canvas, contour0, 1);
  for (Points3::const_iterator it = contour1.begin(); 
       it != contour1.end(); ++it) 
    if (getv<LabelImage3>(canvas, *it) == 1) boundary.push_back(*it);
}



// Given whether a leaf node touches image borders
// Determine whether all nodes touch image borders
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
