#ifndef _n3_tree2d_tree_visit_h_
#define _n3_tree2d_tree_visit_h_

#include "tree2d.h"

namespace n3 {

  namespace tree2d {

    /* Return index of available node (status == 0) with max data value */
    /* Return -1 if no available node */
    template <typename T> int getPick (BiTree<T> const& tree) 
      {
	int i = 0;
	/* Search for first available node */
	while (i < tree.size()) 
	  if (tree[i].status == 0) break;
	  else ++i;
	if (i >= tree.size()) return -1; /* No node available */
	T mv = tree[i].data; /* Maximum data value */
	int mi = i; /* Maximum data value node index */
	while (++i < tree.size()) 
	  if (tree[i].status == 0 && mv < tree[i].data) {
	    mv = tree[i].data;
	    mi = i;
	  }
	return mi;
      }


    /* Only search in indices */
    /* Return index of available node (status == 0) with max data value */
    /* Return -1 if no available node */
    template <typename TTreeData, typename TContainer> int 
      getPick (BiTree<TTreeData> const& tree, TContainer const& indices)
      {
	int mi = -1;
	TTreeData mv;
	bool first = true;
	for (typename TContainer::const_iterator iit = indices.begin(); 
	     iit != indices.end(); ++iit) {
	  if (tree[*iit].status == 0 && (first || tree[*iit].data > mv)) {
	    first = false;
	    mi = *iit;
	    mv = tree[*iit].data;
	  }
	}
	return mi;
      }


    /* Set ancestor statuses to -1 */
    template <typename T> void 
      removeAncestors (std::list<BiTreeNodeChange<T> >& changes, 
		       BiTree<T> const& tree, int i, int status = -1) 
      {
	std::list<int> ancestors;
	tree.get_ancestors(ancestors, i);
	for (std::list<int>::const_iterator it = ancestors.begin(); 
	     it != ancestors.end(); ++it) {
	  BiTreeNodeChange<T> c(tree[*it]);
	  c.setPost("status", status);
	  changes.push_back(c);
	}
      }


    /* Set descendant statuses to -1 */
    template <typename T> void 
      removeDescendants (std::list<BiTreeNodeChange<T> >& changes, 
			 BiTree<T> const& tree, int i, int status = -1)
      {
	std::list<int> descendants;
	tree.get_descendants(descendants, i);
	for (std::list<int>::const_iterator it = descendants.begin(); 
	     it != descendants.end(); ++it) {
	  BiTreeNodeChange<T> c(tree[*it]);
	  c.setPost("status", status);
	  changes.push_back(c);
	}
      }


    template <typename T> void 
      resolve (std::list<int>& picks, BiTree<T>& tree)
      {
	int i = getPick(tree);
	int order = 1;
	while (i >= 0) {
	  picks.push_back(i);
	  std::list<BiTreeNodeChange<T> > changes;
	  /* Pick this node */
	  BiTreeNodeChange<T> c(tree[i]);
	  c.setPost("status", order++);
	  changes.push_back(c);
	  /* Remove ancestors */
	  removeAncestors(changes, tree, i, -1);
	  /* Remove descendants */
	  removeDescendants(changes, tree, i, -1);
	  /* Apply changes */
	  tree.do_changes(changes);
	  /* Get next pick */
	  i = getPick(tree);
	}
      }


    template <typename TTreeNodeData, typename TComp> IntPair 
      getPick (std::vector<BiTree<TTreeNodeData> > const& trees, 
	       bool (*comp)(TComp const&, TTreeNodeData const&), 
	       void (*assign)(TComp&, TTreeNodeData const&))
      {
	IntPair ret = std::make_pair(-1, -1);
	TComp maxi = 0.0;
	int tn = trees.size();
	bool notfound = true;
	for (int i = 0; i < tn; ++i) {
	  int nn = trees[i].size();
	  for (int j = 0; j < nn; ++j) {
	    BiTreeNode<TTreeNodeData> const* node = &(trees[i][j]);
	    if (node->status == 0 && 
		(comp(maxi, node->data) || notfound)) {
	      notfound = false;
	      ret = std::make_pair(i, j);
	      assign(maxi, node->data);
	    }
	  }
	}
	return ret;
      }


    /* Remove ancestors of a list of nodes */
    template <typename T> void 
      removeAncestors (std::list<BiTreeNodeChange<T> >& changes, 
		       BiTree<T> const& tree, 
		       std::list<int> const& indices, int status = -1) 
      {
	if (indices.size() == 0) return;
	/* Is node not removed in this procedure */
	std::vector<bool> remain(tree.size(), true);
	for (std::list<int>::const_iterator it = indices.begin(); 
	     it != indices.end(); ++it) {
	  int i = tree[*it].parent;
	  /* If parent was removed in this procedure */
	  /* All upper ancestors must have been removed */
	  while (i >= 0 && remain[i]) { 
	    changes.push_back(BiTreeNodeChange<T>(tree[i]));
	    changes.back().setPost("status", status);
	    remain[i] = false;
	    i = tree[i].parent;
	  }
	}
      }


    template <typename TTreeNodeData, typename TComp> void 
      resolve (std::list<IntPair>& picks, 
	       std::vector<BiTree<TTreeNodeData> >& trees, 
	       bool (*comp)(TComp const&, TTreeNodeData const&), 
	       void (*assign)(TComp&, TTreeNodeData const&))
      {
	/* Find all leaves each tree have for later use */
	for (typename std::vector<BiTree<TTreeNodeData> >::iterator 
	       tit = trees.begin(); tit != trees.end(); ++tit) 
	  tit->update_leaves();
	IntPair pick = getPick(trees, comp, assign);
	int order = 1;
	while (pick.first >= 0 && pick.second >= 0) {
	  int i = pick.first, j = pick.second;
	  picks.push_back(pick);
	  /* In picked tree */
	  std::list<BiTreeNodeChange<TTreeNodeData> > ichanges;
	  ichanges.push_back(BiTreeNodeChange<TTreeNodeData>
			     (trees[i][j]));
	  ichanges.back().setPost("status", order++);
	  removeAncestors(ichanges, trees[i], j, -1);
	  removeDescendants(ichanges, trees[i], j, -1);
	  trees[i].do_changes(ichanges);
	  /* Find descendant leaves */
	  std::list<int> leaves;
	  trees[i].get_leaves(leaves, j);
	  std::set<Label> llabels;
	  for (std::list<int>::const_iterator lit = leaves.begin(); 
	       lit != leaves.end(); ++lit) 
	    llabels.insert(trees[i][*lit].label);
	  /* In each not picked tree */
	  /* Remove those leaves and their ancestors */
	  for (int t = 0; t < trees.size(); ++t) 
	    if (t != i) {
	      BiTree<TTreeNodeData> const* tr = &(trees[t]);
	      /* Leaves have same labels, but different indices */
	      std::list<int> lindices; /* Leaf indices in trees[t] */
	      for (std::vector<int>::const_iterator lit = 
		     tr->leaves.begin(); lit != tr->leaves.end(); ++lit) 
		if (llabels.count((*tr)[*lit].label) > 0) 
		  lindices.push_back(*lit);
	      std::list<BiTreeNodeChange<TTreeNodeData> > tchanges;
	      /* Remove ancestors of leaves */
	      removeAncestors(tchanges, trees[t], lindices, -1);
	      /* Remove leaves */
	      for (std::list<int>::const_iterator lit = lindices.begin(); 
		   lit != lindices.end(); ++lit) {
		tchanges.push_back(BiTreeNodeChange<TTreeNodeData>
				   (trees[t][*lit]));
		tchanges.back().setPost("status", -1);
	      }
	      trees[t].do_changes(tchanges);
	    }
	  /* Get next pick */
	  pick = getPick(trees, comp, assign);
	}
      }
       
  };

};

#endif
