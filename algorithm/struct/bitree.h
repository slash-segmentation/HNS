#ifndef _n3_bitree_h_
#define _n3_bitree_h_

#include "n3_base.h"

namespace n3 {

  template <typename T> class BiTreeNode {

  public:
    int status;
    Label label;
    int self, parent, child0, child1, sibling; /* Indices */
    T data;


    BiTreeNode (int self = -1) : status(0), label(0), self(self), 
      parent(-1), child0(-1), child1(-1), sibling(-1) {}

    
    friend std::ostream& operator << 
      (std::ostream& out, BiTreeNode<T> const& n) {
      out << "self = " << n.self << ", label = " << n.label 
	  << ", parent = " << n.parent << ", child0 = " << n.child0 
	  << ", child1 = " << n.child1 << ", sibling = " << n.sibling 
	  << ", status = " << n.status << std::endl;
      return out;
    }

  };




  template <typename T> class BiTreeNodeChange {

  public:
    int tree;
    BiTreeNode<T> prev;
    BiTreeNode<T> post;

    
    BiTreeNodeChange (BiTreeNode<T> const& prev, int tree = -1) 
      : tree(tree), prev(prev), post(prev) {}


    template <typename TVal> 
      void setPrev (const char* var, TVal val) 
      {
	if (strcmp(var, "status") == 0) prev.status = (int)val;
	else if (strcmp(var, "data") == 0) prev.data = (T)val;
	else if (strcmp(var, "label") == 0) prev.label = (Label)val;
	else if (strcmp(var, "self") == 0) prev.self = (int)val;
	else if (strcmp(var, "parent") == 0) prev.parent = (int)val;
	else if (strcmp(var, "child0") == 0) prev.child0 = (int)val;
	else if (strcmp(var, "child1") == 0) prev.child1 = (int)val;
	else if (strcmp(var, "sibling") == 0) prev.sibling = (int)val;
	else perr("Error: unsupported variable in BiTreeNodeChange...");
      }

    
    template <typename TVal> 
      void setPost (const char* var, TVal val) 
      {
	if (strcmp(var, "status") == 0) post.status = (int)val;
	else if (strcmp(var, "data") == 0) post.data = (T)val;
	else if (strcmp(var, "label") == 0) post.label = (Label)val;
	else if (strcmp(var, "self") == 0) post.self = (int)val;
	else if (strcmp(var, "parent") == 0) post.parent = (int)val;
	else if (strcmp(var, "child0") == 0) post.child0 = (int)val;
	else if (strcmp(var, "child1") == 0) post.child1 = (int)val;
	else if (strcmp(var, "sibling") == 0) post.sibling = (int)val;
	else perr("Error: unsupported variable in BiTreeNodeChange...");
      }

  };




  template <typename T> class BiTree {
  public:
    typedef BiTreeNode<T> TreeNode;
    typedef std::vector<TreeNode> TreeNodes;
    typedef typename TreeNodes::iterator iterator;
    typedef typename TreeNodes::const_iterator const_iterator;
    typedef typename TreeNodes::reverse_iterator reverse_iterator;
    typedef typename TreeNodes::const_reverse_iterator 
      const_reverse_iterator;

  private: 
    TreeNodes body;
    
  public:
    int root;
    std::vector<int> leaves;
    std::map<Label, int> lnmap; /* Mapping from label to node index */

    BiTree (int nodeNum = 0) {
      root = -1; 
      if (nodeNum > 0) body.resize(nodeNum);
    }

    
    BiTreeNode<T>& operator [] (int i) {return body[i];}


    BiTreeNode<T> const& operator [] (int i) const {return body[i];}


    iterator begin () {return body.begin();}


    const_iterator begin () const {return body.begin();}


    iterator end () {return body.end();}


    const_iterator end () const {return body.end();}


    reverse_iterator rbegin (){return body.rbegin();}


    const_reverse_iterator rbegin () const {return body.rbegin();}


    reverse_iterator rend () {return body.rend();}


    const_reverse_iterator rend () const {return body.rend();}


    unsigned int size () const {return body.size();}


    void resize (int n) {body.resize(n);}


    void reserve (int n) {body.reserve(n);}


    void clear () {body.clear();}


    void push_back (BiTreeNode<T> const& n) {body.push_back(n);}


    void pop_back (BiTreeNode<T> const& n) {body.pop_back(n);}


    BiTreeNode<T>& front () {return body.front();}


    BiTreeNode<T> const& front () const {return body.front();}


    BiTreeNode<T>& back () {return body.back();}


    BiTreeNode<T> const& back () const {return body.back();}



    void update_root () {
      for (BiTree::const_reverse_iterator it = rbegin(); 
	   it != rend(); ++it) 
	if (it->parent < 0) {
	  root = it->self;
	  return;
	}
    }


    void update_leaves () {
      leaves.clear();
      leaves.reserve((body.size() + 1) / 2);
      for (BiTree::const_iterator it = begin(); it != end(); ++it)
	if (it->child0 < 0 && it->child1 < 0) 
	  leaves.push_back(it->self);
    }


    void update_lnmap () {
      for (int i = 0; i < body.size(); ++i) lnmap[body[i].label] = i;
    }


    int get_sibling (int i) {
      if (i >= 0) {
	int p = body[i].parent;
	if (p >= 0) {
	  int c0 = body[p].child0;
	  int c1 = body[p].child1;
	  return i == c0? c1: c0;
	}
      }
      return -1;
    }


    template <typename TContainer> void 
      get_ancestors (TContainer& ancestors,  int i) const 
      {
	i = body[i].parent;
	while (i >= 0) {
	  ancestors.insert(ancestors.end(), i);
	  i = body[i].parent;	
	}
      }


    template <typename TContainer> void 
      get_descendants (TContainer& descendants, int i) const 
      {
	std::queue<int> q;
	if (body[i].child0 >= 0) q.push(body[i].child0);
	if (body[i].child1 >= 0) q.push(body[i].child1);
	while (!q.empty()) {
	  i = q.front();
	  q.pop();
	  descendants.insert(descendants.end(), i);
	  if (body[i].child0 >= 0) q.push(body[i].child0);
	  if (body[i].child1 >= 0) q.push(body[i].child1);
	}
      }


    /* Get all descendant leaves of node i */
    /* Include i if i is a leaf */
    void get_leaves (std::list<int>& leaves, int i) const {
      if (body[i].child0 < 0 && body[i].child1 < 0) {
	leaves.push_back(i);	/* i is a leaf, no descendants */
	return;
      }
      std::queue<int> q;
      if (body[i].child0 >= 0) q.push(body[i].child0);
      if (body[i].child1 >= 0) q.push(body[i].child1);      
      while (!q.empty()) {
	i = q.front();
	q.pop();
	if (body[i].child0 < 0 && body[i].child1 < 0)
	  leaves.push_back(i);	  
	else {
	  if (body[i].child0 >= 0) q.push(body[i].child0);
	  if (body[i].child1 >= 0) q.push(body[i].child1);
	}
      }
    }


    void do_change (BiTreeNodeChange<T> const& c) {
      body[c.prev.self] = c.post;
    }


    void undo_change (BiTreeNodeChange<T> const& c) {
      body[c.post.self] = c.prev;
    }


    void do_changes (std::list<BiTreeNodeChange<T> > const& cs) {
      for (typename std::list<BiTreeNodeChange<T> >::const_iterator 
	     it = cs.begin(); it != cs.end(); ++it) do_change(*it);
    }


    void undo_changes (std::list<BiTreeNodeChange<T> > const& cs) {
      for (typename std::list<BiTreeNodeChange<T> >::const_iterator 
	     it = cs.begin(); it != cs.end(); ++it) undo_change(*it);
    }


    friend std::ostream& operator << 
      (std::ostream& out, BiTree<T> const& tree) {
      for (const_iterator it = tree.begin(); it != tree.end(); ++it)
	out << *it << std::endl;
      return out;
    }
    
  };

};

#endif
