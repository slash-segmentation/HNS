#ifndef _n3_tree2d_util_h_
#define _n3_tree2d_util_h_

#include "util/stat_util.h"
#include "util/struct_util.h"
#include "tree2d.h"

namespace n3 {

  namespace tree2d {

    typedef std::map<LabelPair, std::list<fPixel> > BoundaryTable;
    typedef std::priority_queue<fMerge> MergeQueue;

    // Used in pre-merging
    Label getMinBoundaryNeighbor (BoundaryTable const& bt, Label r0);

    // Do 2 types of merge
    // Input label image will be modified
    LabelImage::Pointer merge (LabelImage::Pointer labelImage, 
			       FloatImage::Pointer pbImage, 
			       int ath0, int ath1, double pth);

    // -median
    Float getSaliency (std::list<fPixel> const& pixels);

    // In key <r0, r1> keep r0 < r1
    void getBoundaryTable (BoundaryTable& bt,
			   PointLabelMap const& lmap, 
			   FloatImage::Pointer valImage);

    // In key <r0, r1> keep r0 < r1
    void getBoundaryTable (BoundaryTable& bt, MergeQueue& mq, 
			   PointLabelMap const& lmap, 
			   FloatImage::Pointer valImage);

    // Assume r01 is greater than any existing region label
    void updateBoundaryTable (BoundaryTable& bt, MergeQueue& mq, 
			      std::set<LabelPair>& removed, 
			      Label r0, Label r1, Label r01);

    // Merge from rfrom to rto
    void updateBoundaryTable (BoundaryTable& bt, 
			      PointLabelMap const& lmap, 
			      Label rfrom, Label rto);

    Label getMaxLabel (PointLabelMap const& lmap);

    // bp: all 0 pixel points
    void getMerges (std::list<fMerge>& merges, Points const& bp,
		    LabelImage::Pointer labelImage, 
		    FloatImage::Pointer valImage);

    void getMerges (std::list<fMerge>& merges, 
		    LabelImage::Pointer labelImage, 
		    FloatImage::Pointer valImage);

    void getBoundary (Points& boundary, Points const& contour0, 
		      Points const& contour1, 
		      LabelImage::Pointer& canvas);

    void updateLeafSaliencies (fTree& tree, PointMap const& rmap, 
			       FloatImage::Pointer valImage);

    void updateLeafSaliencies (fTree& tree, double val);

    // mprobs: list of merge probabilities
    // penalizeType: 0 - squared, 1 - half
    void getPotentials (fTree& tree, std::list<double> const& mprobs, 
			int penalizeType = 0);

    /* Generate hierarchical region and contour point map */
    template <typename T> void
      getPointMap (PointMap& rmap, PointMap& cmap, 
		   LabelImage::Pointer im, 
		   std::list<Merge<T> > const& merges, 
		   bool includeBG, int connect, bool keepSrc)
      {
	PointLabelMap lmap;
	getPointMap(rmap, im, true);
	getPointNeighbors(lmap, rmap[BGVAL], im, connect);
	getNeighborPoints(cmap, lmap, true);
	for (typename std::list<Merge<T> >::const_iterator it = 
	       merges.begin();it != merges.end(); ++it)
	  merge(rmap, cmap, lmap, it->from0, it->from1, it->to, 
		true, keepSrc);
	if (!includeBG) rmap.erase(BGVAL);
      }


    /* Generate hierarchical region point map */
    template <typename T> void
      getPointMap (PointMap& rmap, LabelImage::Pointer im, 
		   std::list<Merge<T> > const& merges, 
		   bool includeBG, int connect, bool keepSrc)
      {
	PointMap cmap;
	getPointMap(rmap, cmap, im, merges, includeBG, connect, keepSrc);
      }



    /* Generate tree from merge list */
    /* For fMerge, leafMergeData would be initial water level */
    /* If merge data is not considered set NULL assign function pointer */
    /* And set leafMergeData to arbitrary value */
    template <typename TTreeData, typename TMergeData> void 
      getTree (BiTree<TTreeData>& tree, 
	       std::list<Merge<TMergeData> > const& merges, 
	       void (*assign)(TTreeData&, TMergeData const&) = NULL)
      {
	int nodeNum = 2 * merges.size() + 1;
	tree.resize(nodeNum);
	std::map<Label, int> indexMap;
	int i = 0;
	for (std::list<fMerge>::const_iterator it = merges.begin(); 
	     it != merges.end(); ++it) {
	  std::map<Label, int>::iterator fit0 = indexMap.find(it->from0);
	  std::map<Label, int>::iterator fit1 = indexMap.find(it->from1);
	  if (fit0 == indexMap.end()) {
	    indexMap[it->from0] = i;
	    tree[i].label = it->from0;
	    tree[i].self = i;
	    tree[i].child0 = tree[i].child1 = tree[i].sibling = -1;
	    if (assign != NULL) (*assign)(tree[i].data, 0.0);
	    ++i;
	  }
	  if (fit1 == indexMap.end()) {
	    indexMap[it->from1] = i;
	    tree[i].label = it->from1;
	    tree[i].self = i;
	    tree[i].child0 = tree[i].child1 = tree[i].sibling = -1;
	    if (assign != NULL) (*assign)(tree[i].data, 0.0);
	    ++i;
	  }
	  indexMap[it->to] = i;
	  tree[i].label = it->to;
	  tree[i].self = i;
	  tree[i].parent = tree[i].sibling = -1;
	  tree[i].child0 = indexMap[it->from0];
	  tree[i].child1 = indexMap[it->from1];
	  if (assign != NULL) (*assign)(tree[i].data, it->data);
	  tree[tree[i].child0].parent = i;
	  tree[tree[i].child1].parent = i;
	  tree[tree[i].child0].sibling = tree.get_sibling(tree[i].child0);
	  tree[tree[i].child1].sibling = tree.get_sibling(tree[i].child1);
	  ++i;
	}
      }



    /* Truncate tree: trunc[i] = true if node[i] is to be removed */
    /* Assume if a node is truncated, all its children are truncated */
    /* Input tree is to be changed */
    /* If merge data is not considered set NULL assign function pointer */
    template <typename TTreeDataIn, typename TTreeDataOut> void 
      truncTree (BiTree<TTreeDataOut>& ttree, 
		 std::vector<bool>& trunc, BiTree<TTreeDataIn>& tree, 
		 void (*assign)(TTreeDataOut&, TTreeDataIn const&) = NULL)
      {
	for (int i = tree.size() - 1; i >= 0; --i) {
	  int c0 = tree[i].child0, c1 = tree[i].child1;
	  if (!trunc[i] && c0 >= 0 && c1 >= 0) {
	    /* If two children are both to truncate, remove them both */
	    if (trunc[c0] && trunc[c1]) {
	      tree[i].child0 = -1; 
	      tree[i].child1 = -1;
	    }
	    /* If only child A is to truncate, remove both children */
	    /* And take children of child B */
	    else if (trunc[c0]) {
	      trunc[c1] = true;
	      tree[i].child0 = tree[c1].child0;
	      tree[i].child1 = tree[c1].child1;
	      if (tree[i].child0 >= 0) tree[tree[i].child0].parent = i;
	      if (tree[i].child1 >= 0) tree[tree[i].child1].parent = i;
	      ++i;
	    }
	    else if (trunc[c1]) {
	      trunc[c0] = true;
	      tree[i].child0 = tree[c0].child0;
	      tree[i].child1 = tree[c0].child1;
	      if (tree[i].child0 >= 0) tree[tree[i].child0].parent = i;
	      if (tree[i].child1 >= 0) tree[tree[i].child1].parent = i;
	      ++i;
	    }
	  }
	}
	int tn = 0; /* Size of new tree */
	for (std::vector<bool>::const_iterator it = trunc.begin(); 
	     it != trunc.end(); ++it) if (!*it) ++tn; 
	ttree.resize(tn);
	int t = 0; /* Index for new tree */
	/* Old to new index mapping */
	std::vector<int> indexMap(tree.size(), -1); 
	for (int i = 0; i < tree.size(); ++i) {
	  if (!trunc[i]) {
	    indexMap[i] = t;
	    BiTreeNode<TTreeDataOut>* pt = &(ttree[t]);
	    BiTreeNode<TTreeDataIn>* pi = &(tree[i]);
	    pt->self = t;
	    pt->label = pi->label; /* Label stays */
	    pt->parent = -1;
	    if (assign != NULL) (*assign)(pt->data, pi->data);
	    if (pi->child0 >= 0) {
	      pt->child0 = indexMap[pi->child0];
	      ttree[pt->child0].parent = t;
	    }
	    else pt->child0 = -1;
	    if (pi->child1 >= 0) {
	      pt->child1 = indexMap[pi->child1];
	      ttree[pt->child1].parent = t;
	      ttree[pt->child1].sibling = ttree.get_sibling(pt->child1);
	    }
	    else pt->child1 = -1;
	    ++t;
	  }
	}
	/* Update siblings */
	for (int i = 0; i < ttree.size(); ++i) {
	  BiTreeNode<TTreeDataOut>* pt = &(ttree[i]);
	  if (pt->child0 >= 0) 
	    ttree[pt->child0].sibling = ttree.get_sibling(pt->child0);
	  if (pt->child1 >= 0) 
	    ttree[pt->child1].sibling = ttree.get_sibling(pt->child1);
	}
      }



    /* Tree leaves are to be updated */
    template <typename TTreeData> LabelImage::Pointer 
      paintLeaf (BiTree<TTreeData>& tree, PointMap const& rmap, 
		 int width, int height)
      {
	LabelImage::Pointer ret = 
	  createImage<LabelImage>(width, height, 0);
	tree.update_leaves();
	for (std::vector<int>::const_iterator it = tree.leaves.begin(); 
	     it != tree.leaves.end(); ++it) {
	  PointMap::const_iterator r = rmap.find(tree[*it].label);
	  if (r != rmap.end()) 
	    setvs<LabelImage>(ret, r->second, r->first);
	  else perr("Error: no leaf region points found...");
	}
	return ret;
      }
    
  };

};

#endif
