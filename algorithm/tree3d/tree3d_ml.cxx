#include "tree3d_ml.h"
#include "bc.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

// Saliency = merge probability
// Since we use priority queue (top is always 'largest')
// We put merge probability as merge value
// But output 'saliency' is actually split probability (1 - saliency)
void n3::tree3d::getBoundaryTable (fBoundaryTable3& bt, 
				   PointLabelMap3 const& lmap, 
				   PointMap3 const& rmap, 
				   PointMap3 const& cmap, 
				   PointMap3 const& bmap, 
				   LabelImage3::Pointer canvas, 
				   FloatImage3::Pointer rawImage, 
				   FloatImage3::Pointer pbImage, 
				   rf::Model const& rfModel)
{
  // Add boundary table entries by finding neighbor pairs
  // Remove possible merges between two regions without any contour points
  // That are exclusively between the two regions
  std::set<LabelPair> pairsWithExclusivePoint;
  for (PointLabelMap3::const_iterator pit = lmap.begin(); 
       pit != lmap.end(); ++pit) 
    for (LabelSet::const_iterator lit0 = pit->second.begin(); 
	 lit0 != pit->second.end(); ++lit0) {
      LabelSet::const_iterator lit1 = lit0; ++lit1;
      while (lit1 != pit->second.end()) {
	LabelPair key = *lit0 < *lit1? std::make_pair(*lit0, *lit1): 
	  std::make_pair(*lit1, *lit0);
	bt.insert(bt.end(), 
		  std::make_pair(key, std::make_pair(flist(), -1.0)));
	if (pit->second.size() == 2) 
	  pairsWithExclusivePoint.insert(pairsWithExclusivePoint.end(), 
					 key);
	++lit1;
      }
    }
  std::list<fBoundaryTable3::iterator> rms;
  std::set<LabelPair>::const_iterator ppit = 
    pairsWithExclusivePoint.begin();
  fBoundaryTable3::iterator btit = bt.begin();
  while (ppit != pairsWithExclusivePoint.end() && btit != bt.end())
    if (*ppit == btit->first) {
      ++ppit;
      ++btit;
    }
    else {
      rms.push_back(btit);
      ++btit;
    }
  remove(bt, rms);
  // // For debug
  // std::cerr << "done adding bt entries" << std::endl;
  // // ~ For debug
  // Compute boundary prediction
  for (fBoundaryTable3::iterator tit = bt.begin(); tit != bt.end(); 
       ++tit) {
    PointMap3::const_iterator 
      bit0 = bmap.find(tit->first.first), 
      bit1 = bmap.find(tit->first.second);
    getBoundaryFeatures(tit->second.first, 
			&(rmap.find(tit->first.first)->second), 
			&(rmap.find(tit->first.second)->second), 
			&(cmap.find(tit->first.first)->second), 
			&(cmap.find(tit->first.second)->second), 
			bit0 == bmap.end()? NULL: &(bit0->second), 
			bit1 == bmap.end()? NULL: &(bit1->second), 
			canvas, rawImage, pbImage);
    // // For debug
    // std::cerr << "done getting bcfeat for iseg pair (" 
    // 	      << tit->first.first << ", " << tit->first.second << ") ";
    // // ~ For debug
    tit->second.second = 
      rf::predict(rfModel, -1, tit->second.first);
    // // For debug
    // std::cerr << "and predicting" << std::endl;
    // // ~ For debug
  }
}



void n3::tree3d::getBoundaryTable (fBoundaryTable3& bt, MergeQueue& mq, 
				   PointLabelMap3 const& lmap, 
				   PointMap3 const& rmap, 
				   PointMap3 const& cmap, 
				   PointMap3 const& bmap, 
				   LabelImage3::Pointer canvas, 
				   FloatImage3::Pointer rawImage, 
				   FloatImage3::Pointer pbImage, 
				   rf::Model const& rfModel)
{
  getBoundaryTable(bt, lmap, rmap, cmap, bmap, canvas, 
		   rawImage, pbImage, rfModel);
  for (fBoundaryTable3::const_iterator tit = bt.begin(); 
       tit != bt.end(); ++tit) 
    mq.push(fMerge(tit->first.first, tit->first.second, 0, 
		   tit->second.second));
}



// Collect features if pFeats != NULL
// Collect labels if pTruthImage != NULL && pLabels != NULL
// Use mq = NULL to skip updating it
// Use removed = NULL to skip collecting removed label pairs
void n3::tree3d::updateBoundaryTable (std::list<flist>* pFeats, 
				      std::list<int>* pLabels, 
				      fBoundaryTable3& bt, MergeQueue* mq, 
				      std::set<LabelPair>* removed, 
				      PointMap3& rmap, PointMap3& cmap, 
				      PointLabelMap3& lmap, 
				      PointMap3& bmap, Label l0, 
				      Label l1, Label l01, 
				      LabelImage3::Pointer canvas, 
				      FloatImage3::Pointer rawImage, 
				      FloatImage3::Pointer pbImage, 
				      rf::Model const& rfModel, 
				      LabelImage3::Pointer* pTruthImage)
{
  if (l0 > l1) std::swap(l0, l1);
  LabelPair key = std::make_pair(l0, l1);
  fBoundaryTable3::iterator btit = bt.find(key);
  if (pFeats != NULL) {	// Collect features
    pFeats->push_back(flist());
    splice(pFeats->back(), btit->second.first, false);
  }
  if (removed != NULL) removed->insert(key);
  bt.erase(btit);
  // // For debug
  // std::cerr << "done bt.erase(btit)" << std::endl;
  // // ~ For debug
  std::list<fBoundaryTable3::iterator> erm; // Entries to remove
  std::set<Label> neighbors;
  for (fBoundaryTable3::iterator tit = bt.begin(); tit != bt.end(); ++tit)
    if (tit->first.first == l0 || tit->first.first == l1) {
      if (removed != NULL) removed->insert(tit->first);
      erm.push_back(tit);
      neighbors.insert(tit->first.second);
    }
    else if (tit->first.second == l0 || tit->first.second == l1) {
      if (removed != NULL) removed->insert(tit->first);
      erm.push_back(tit);
      neighbors.insert(tit->first.first);
    }
  remove(bt, erm); // Remove all entries related to l0/l1
  // // For debug
  // std::cerr << "done remove(bt, erm)" << std::endl;
  // // ~ For debug
  Points3 const* r01 = NULL;
  std::list<const Points3*> sub, sup;
  if (pTruthImage != NULL && pLabels != NULL) { // Collect labels
    sub.push_back(&(rmap.find(l0)->second));
    sub.push_back(&(rmap.find(l1)->second));
    Points3 ub;
    getBoundary(ub, cmap.find(l0)->second, cmap.find(l1)->second, canvas);
    std::vector<BigFloat> 
      subScores = getPairScores(sub, &ub, *pTruthImage); 
    merge(rmap, cmap, lmap, bmap, l0, l1, l01, true, false);
    // // For debug
    // std::cerr << "done merging rmap etc." << std::endl;
    // // ~ For debug
    r01 = &(rmap.find(l01)->second);
    // // For debug
    // std::cerr << "done finding r01" << std::endl;
    // // ~ For debug
    sup.push_back(r01);
    // // For debug
    // std::cerr << "done getting ub" << std::endl;
    // // ~ For debug
    std::vector<BigFloat> 
      supScores = getPairScores(sup, &ub, *pTruthImage);
    // Boundary is true; do not merge
    int label = 1;
    // Boundary is false; merge
    if (subScores[2] < supScores[2] || 
	(subScores[0] < EPSILON && subScores[1] < EPSILON && 
	 supScores[0] < EPSILON && supScores[1] < EPSILON) || 
	(subScores[2] == supScores[2] && subScores[0] > 0.9 && 
	 supScores[0] > 0.9)) label = -1;
    pLabels->push_back(label);
    // // For debug
    // std::cerr << "done getting bclabel" << std::endl;
    // // ~ For debug
  }
  else {
    merge(rmap, cmap, lmap, bmap, l0, l1, l01, true, false);
    r01 = &(rmap.find(l01)->second);
  }
  Points3 const* c01 = &(cmap.find(l01)->second);
  for (std::set<Label>::const_iterator nit = neighbors.begin(); 
       nit != neighbors.end(); ++nit) {
    key = std::make_pair(*nit, l01);
    fBoundaryTable3::iterator tit = 
      bt.insert(std::make_pair(key, std::make_pair(flist(), -1.0))).first;
    PointMap3::const_iterator bit0 = bmap.find(*nit), 
      bit1 = bmap.find(l01);
    getBoundaryFeatures(tit->second.first, &(rmap.find(*nit)->second), 
			r01, &(cmap.find(*nit)->second), c01, 
			bit0 == bmap.end()? NULL: &(bit0->second), 
			bit1 == bmap.end()? NULL: &(bit1->second), 
			canvas, rawImage, pbImage);
    tit->second.second = rf::predict(rfModel, -1, tit->second.first);
    if (mq != NULL) mq->push(fMerge(*nit, l01, 0, tit->second.second));
    // // For debug
    // std::cerr << "done push merge queue" << std::endl;
    // // ~ For debug
  }
}



// Generate merges
// Collect features if pFeats != NULL
// Collect labels if pTruthImage != NULL && pLabels != NULL
void n3::tree3d::getMerges (std::list<fMerge>& merges, 
			    std::list<flist>* pFeats, 
			    std::list<int>* pLabels, PointMap3& rmap, 
			    PointMap3& cmap, PointLabelMap3& lmap, 
			    PointMap3& bmap, LabelImage3::Pointer canvas, 
			    FloatImage3::Pointer rawImage, 
			    FloatImage3::Pointer pbImage, 
			    rf::Model const& rfModel, 
			    LabelImage3::Pointer* pTruthImage)
{
  fBoundaryTable3 bt;
  MergeQueue mq;
  getBoundaryTable(bt, mq, lmap, rmap, cmap, bmap, canvas, 
		   rawImage, pbImage, rfModel);
  // // For debug
  // std::cerr << "done getBoundaryTable" << std::endl;
  // // ~ For debug
  std::set<LabelPair> removed;
  Label l01 = getMaxLabel(lmap) + 1;
  while (mq.size() > 0) {
    fMerge m = mq.top();
    mq.pop();
    std::set<LabelPair>::iterator rit = 
      removed.find(std::make_pair(m.from0, m.from1));
    if (rit == removed.end()) {
      Label l0 = m.from0, l1 = m.from1;
      updateBoundaryTable(pFeats, pLabels, bt, &mq, &removed, rmap, 
			  cmap, lmap, bmap, l0, l1, l01, canvas, 
			  rawImage, pbImage, rfModel, pTruthImage);
      // Saliency = merge probability; output 1 - saliency
      merges.push_back(fMerge(l0, l1, l01, 1.0 - m.data));
      ++l01;
      // // For debug
      // std::cerr << "done merging: " << l0 << " + " << l1
      // 		<< " -> " << l01 << std::endl;
      // // ~ For debug
    }
    else removed.erase(rit);
  }
}
