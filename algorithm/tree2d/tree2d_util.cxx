#include "tree2d_util.h"
using namespace n3;
using namespace n3::tree2d;

// -median
Float n3::tree2d::getSaliency (std::list<fPixel> const& pixels)
{
  int len = pixels.size();
  std::list<fPixel>::const_iterator it = pixels.begin();
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
void n3::tree2d::getBoundaryTable (BoundaryTable& bt,
				   PointLabelMap const& lmap, 
				   FloatImage::Pointer valImage)
{
  for (PointLabelMap::const_iterator pit = lmap.begin(); 
       pit != lmap.end(); ++pit) 
    for (LabelSet::const_iterator lit0 = pit->second.begin(); 
	 lit0 != pit->second.end(); ++lit0) {
      LabelSet::const_iterator lit1 = lit0; ++lit1;
      while (lit1 != pit->second.end()) {
	LabelPair key = *lit0 < *lit1? std::make_pair(*lit0, *lit1): 
	  std::make_pair(*lit1, *lit0);
	bt[key].push_back(fPixel(pit->first, 
				 getv<FloatImage>(valImage, 
						   pit->first)));
	++lit1;
      }
    }
  // Remove possible merges between two regions without any contour points
  // That are exclusively between the two regions
  std::list<BoundaryTable::iterator> rms;
  for (BoundaryTable::iterator bit = bt.begin(); bit != bt.end(); ++bit) {
    bool isExclusivePointFound = false;
    for (std::list<fPixel>::const_iterator pit = bit->second.begin(); 
	 pit != bit->second.end(); ++pit) {
      PointLabelMap::const_iterator lit = lmap.find(*pit);
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
  for (BoundaryTable::iterator tit = bt.begin(); tit != bt.end(); ++tit)
    tit->second.sort();
}



// In key <r0, r1> keep r0 < r1
void n3::tree2d::getBoundaryTable (BoundaryTable& bt, MergeQueue& mq, 
				   PointLabelMap const& lmap, 
				   FloatImage::Pointer valImage)
{
  getBoundaryTable(bt, lmap, valImage);
  for (BoundaryTable::iterator tit = bt.begin(); tit != bt.end(); ++tit)
    mq.push(fMerge(tit->first.first, tit->first.second, 0, 
		   getSaliency(tit->second)));
}



// Assume r01 is greater than any existing region label
void n3::tree2d::updateBoundaryTable (BoundaryTable& bt, MergeQueue& mq, 
				      std::set<LabelPair>& removed, 
				      Label r0, Label r1, Label r01)
{
  if (r0 > r1) std::swap(r0, r1); // Keep r0 < r1
  removed.insert(std::make_pair(r0, r1));
  bt.erase(std::make_pair(r0, r1));
  std::list<BoundaryTable::iterator> erm; // Entries to remove
  std::map<Label, std::list<fPixel> > neighbors;
  for (BoundaryTable::iterator it = bt.begin(); it != bt.end(); ++it) 
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
  for (std::map<Label, std::list<fPixel> >::iterator nit = 
	 neighbors.begin(); nit != neighbors.end(); ++nit) {
    LabelPair lp = std::make_pair(nit->first, r01);
    std::list<fPixel>* pp = &(bt[lp]);
    merge(*pp, nit->second, true);
    mq.push(fMerge(lp.first, lp.second, 0, getSaliency(*pp)));
  }
}



// Merge from rfrom to rto
void n3::tree2d::updateBoundaryTable (BoundaryTable& bt, 
				      PointLabelMap const& lmap, 
				      Label rfrom, Label rto)
{
  if (rfrom > rto) bt.erase(std::make_pair(rto, rfrom));
  else bt.erase(std::make_pair(rfrom, rto));
  std::list<BoundaryTable::iterator> erm;
  std::map<Label, std::list<fPixel> > neighbors;
  for (BoundaryTable::iterator it = bt.begin(); it != bt.end(); ++it) {
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
  for (std::map<Label, std::list<fPixel> >::iterator it = 
	 neighbors.begin(); it != neighbors.end(); ++it) {
    LabelPair key = (it->first < rto? std::make_pair(it->first, rto): 
		     std::make_pair(rto, it->first));
    merge(bt[key], it->second, true);
  }
}



Label n3::tree2d::getMaxLabel (PointLabelMap const& lmap)
{
  Label ret = 0;
  for (PointLabelMap::const_iterator lit = lmap.begin(); 
       lit != lmap.end(); ++lit)
    for (LabelSet::const_iterator sit = lit->second.begin(); 
	 sit != lit->second.end(); ++sit) 
      if (*sit > ret) ret = *sit;
  return ret;
}



// bp: all 0 pixel points
void n3::tree2d::getMerges (std::list<fMerge>& merges, Points const& bp,
			    LabelImage::Pointer labelImage, 
			    FloatImage::Pointer valImage)
{
  PointLabelMap lmap;
  getPointNeighbors(lmap, bp, labelImage, CRCONN);
  BoundaryTable bt;
  MergeQueue mq;
  getBoundaryTable(bt, mq, lmap, valImage);
  std::set<LabelPair> removed;
  Label r01 = getMaxLabel(lmap) + 1;
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
    }
    else removed.erase(rit);
  }
}



void n3::tree2d::getMerges (std::list<fMerge>& merges, 
			    LabelImage::Pointer labelImage, 
			    FloatImage::Pointer valImage)
{
  Points bp;
  getPoints(bp, labelImage, BGVAL);
  getMerges(merges, bp, labelImage, valImage);
}


void n3::tree2d::getBoundary (Points& boundary, Points const& contour0, 
			      Points const& contour1, 
			      LabelImage::Pointer& canvas)
{
  canvas->FillBuffer(0);
  setvs<LabelImage>(canvas, contour0, 1);
  for (Points::const_iterator it = contour1.begin(); 
       it != contour1.end(); ++it) 
    if (getv<LabelImage>(canvas, *it) == 1) boundary.push_back(*it);
}



// // In key <r0, r1> keep r0 < r1
// void n3::tree2d::getBoundaryTable (BoundaryTable& bt, 
// 				   PointLabelMap const& lmap)
// {
//   for (PointLabelMap::const_iterator pit = lmap.begin(); 
//        pit != lmap.end(); ++pit)
//     for (LabelSet::const_iterator lit0 = pit->second.begin(); 
// 	 lit0 != pit->second.end(); ++lit0) {
//       LabelSet::const_iterator lit1 = lit0; ++lit1;
//       for (; lit1 != pit->second.end(); ++lit1) {
// 	LabelPair key = *lit0 < *lit1? std::make_pair(*lit0, *lit1): 
// 	  std::make_pair(*lit1, *lit0);
// 	bt[key].push_back(pit->first);
//       }
//     }
//   // Remove possible merges between two regions without any contour points
//   // That are exclusively between the two regions
//   std::list<BoundaryTable::iterator> rms;
//   for (BoundaryTable::iterator bit = bt.begin(); bit != bt.end(); ++bit) {
//     bool isExclusivePointFound = false;
//     for (Points::const_iterator pit = bit->second.begin(); 
// 	 pit != bit->second.end(); ++pit) {
//       PointLabelMap::const_iterator lit = lmap.find(*pit);
//       if (lit->second.size() == 2 && 
// 	  lit->second.count(bit->first.first) > 0 && 
// 	  lit->second.count(bit->first.second) > 0) {
// 	isExclusivePointFound = true; 
// 	break;
//       }
//     }
//     if (!isExclusivePointFound) rms.push_back(bit);
//   }
//   remove(bt, rms);
// }



// // Assume r01 is greater than any existing region label
// void n3::tree2d::updateBoundaryTable (BoundaryTable& bt, 
// 				      Label r0, Label r1, Label r01)
// {
//   if (r0 > r1) std::swap(r0, r1); // Keep r0 < r1
//   bt.erase(std::make_pair(r0, r1));
//   std::list<BoundaryTable::iterator> erm; // Entries to remove
//   std::map<Label, Points> neighbors;
//   for (BoundaryTable::iterator it = bt.begin(); it != bt.end(); ++it)
//     if (it->first.first == r0 || it->first.first == r1) {
//       erm.push_back(it);
//       splice(neighbors[it->first.second], it->second, false);
//     }
//     else if (it->first.second == r0 || it->first.second == r1) {
//       erm.push_back(it);
//       splice(neighbors[it->first.first], it->second, false);
//     }
//   remove(bt, erm); // Remove all entries related to r0/r1
//   for (std::map<Label, Points>::iterator it = neighbors.begin(); 
//        it != neighbors.end(); ++it)
//     splice(bt[std::make_pair(it->first, r01)], it->second, true);
// }



// // bp: all 0 pixel points
// // metric: 1 - min, 2 - mean, 3 - median
// void n3::tree2d::getMerges (std::list<fMerge>& merges, 
// 			    PointMap& cmap, Points const& bp, 
// 			    LabelImage::Pointer labelImage, 
// 			    FloatImage::Pointer valImage, int metric)
// {
//   PointLabelMap lmap;
//   getPointNeighbors(lmap, bp, labelImage, CRCONN);
//   getNeighborPoints(cmap, lmap);
//   BoundaryTable bt;
//   getBoundaryTable(bt, lmap);
//   Label r01 = cmap.rbegin()->first; ++r01;
//   while (bt.size() > 0) {
//     Float minv = FLOAT_MAX;
//     Label r0, r1;
//     for (BoundaryTable::const_iterator it = bt.begin(); it != bt.end(); 
// 	 ++it) {
//       // Use certain metric to determine merges
//       std::list<Float> vals;
//       getvs<FloatImage>(vals, valImage, it->second);
//       Float v;
//       if (metric == 1) v = getMin(vals);
//       else if (metric == 2) v = getMean(vals);
//       else if (metric == 3) v = getMedian(vals);
//       else perr("Error: unsupported boundary metric...");
//       if (v < minv) {
// 	minv = v;
// 	r0 = it->first.first;
// 	r1 = it->first.second;
//       }
//     }
//     merges.push_back(fMerge(r0, r1, r01, minv));
//     Points c2rp;
//     merge(c2rp, cmap, lmap, r0, r1, r01);
//     updateBoundaryTable(bt, r0, r1, r01);
//     ++r01;
//   }
// }



// void n3::tree2d::getMerges (std::list<fMerge>& merges, 
// 			    LabelImage::Pointer labelImage, 
// 			    FloatImage::Pointer valImage, int metric)
// {
//   Points bp;
//   getPoints(bp, labelImage, BGVAL);
//   PointMap cmap;
//   getMerges(merges, cmap, bp, labelImage, valImage, metric);
// }



void n3::tree2d::updateLeafSaliencies (fTree& tree, PointMap const& rmap, 
				       FloatImage::Pointer valImage)
{
  for (fTree::iterator tit = tree.begin(); tit != tree.end(); ++tit)
    if (tit->child0 < 0 && tit->child1 < 0) {
      std::list<Float> vals;
      PointMap::const_iterator pit = rmap.find(tit->label);
      if (pit == rmap.end()) 
	perr("Error: cannot find tree node region...");
      getvs<FloatImage>(vals, valImage, pit->second);
      tit->data = getMin(vals);
    } 
}



void n3::tree2d::updateLeafSaliencies (fTree& tree, double val)
{
  for (fTree::iterator tit = tree.begin(); tit != tree.end(); ++tit)
    if (tit->child0 < 0 && tit->child1) tit->data = val;
}



// mprobs: list of merge probabilities
// penalizeType: 0 - squared, 1 - half
void n3::tree2d::getPotentials 
(fTree& tree, std::list<double> const& mprobs, int penalizeType)
{
  // Assign merge probabilities
  std::list<double>::const_iterator pit = mprobs.begin(); 
  for (fTree::iterator tit = tree.begin(); tit != tree.end(); ++tit) {
    tit->status = 0;
    if (tit->child0 >= 0 && tit->child1 >= 0) {
      double p = *pit;
      if (fabs(p) < EPSILON) p += 1e-4;
      tit->data = p;
      if (tit->data )
      ++pit;
    }
    else tit->data = 1.0;
  }
  // Compute potentials
  for (fTree::iterator tit = tree.begin(); tit != tree.end(); ++tit) 
    if (tit->parent >= 0) {
      double p = tree[tit->parent].data;
      if (fabs(p) < EPSILON) p += 1e-4;
      tit->data = tit->data * (1.0 - p);
    }
  // Penalize leaves and root
  for (fTree::iterator tit = tree.begin(); tit != tree.end(); ++tit)
    if (tit->child0 < 0 || tit->child1 < 0 || tit->parent < 0) 
      if (penalizeType == 0) tit->data *= tit->data;
      else if (penalizeType == 1) tit->data *= 0.5;
      else perr("Error: unsupported penalizing type...");
}
