#ifndef _n3_graph_h_
#define _n3_graph_h_

#include "n3_base.h"
/* #include "boost/unordered_map.hpp" */
/* #include "boost/unordered_set.hpp" */

namespace n3 {

  template <typename TNodeData, typename TEdgeData> class Graph {

  public:

    /* Node */
    template <typename T> class Node {

    public:
      int id;
      std::list<int> in_edges, out_edges;
      T data;

      Node (int id) : id(id) {}

      Node (int id, T const& data) : id(id), data(data) {}

      friend std::ostream& operator << (std::ostream& os, 
					Node const& node) {
	os << node.id << " " << node.in_edges.size() << " ";
	for (std::list<int>::const_iterator it = node.in_edges.begin(); 
	     it != node.in_edges.end(); ++it) os << *it << " ";
	os << node.out_edges.size() << " ";
	for (std::list<int>::const_iterator it = node.out_edges.begin(); 
	     it != node.out_edges.end(); ++it) os << *it << " ";
	os << node.data << " ";
	return os;
      }

      friend std::istream& operator >> (std::istream& is, Node& node) {
	int n;
	is >> node.id >> n;
	for (int i = 0; i < n; ++i) {
	  int edge;
	  is >> edge;
	  node.in_edges.push_back(edge);
	}
	is >> n;
	for (int i = 0; i < n; ++i) {
	  int edge;
	  is >> edge;
	  node.out_edges.push_back(edge);
	}
	is >> node.data;
	return is;
      }

    };

    /* Edge */
    template <typename T> class Edge {

    public:
      int id;
      int src_node, tar_node;
      TEdgeData data;

      Edge (int id = -1, int src_node = -1, int tar_node = -1) 
	: id(id), src_node(src_node), tar_node(tar_node) {}

      Edge (int id, int src_node, int tar_node, T const& data) 
	: id(id), src_node(src_node), tar_node(tar_node), data(data) {}

      friend std::ostream& operator << (std::ostream& os, 
					Edge const& edge) {
	os << edge.id << " " << edge.src_node << " " 
	   << edge.tar_node << " " << edge.data << " ";
	return os;
      }

      friend std::istream& operator >> (std::istream& is, Edge& edge) {
	is >> edge.id >> edge.src_node >> edge.tar_node >> edge.data;
	return is;
      }

    };

    /* typedef boost::unordered_map<int, Node<TNodeData> > NodeMap; */
    /* typedef boost::unordered_map<int, Edge<TEdgeData> > EdgeMap; */
    typedef std::map<int, Node<TNodeData> > NodeMap;
    typedef std::map<int, Edge<TEdgeData> > EdgeMap;

    int id;
    int max_node_id, max_edge_id;
    NodeMap nodes;
    EdgeMap edges;

    Graph (int id) : id(id), max_node_id(0), max_edge_id(0) {}

    int add_node () {
      int nid = ++max_node_id;
      nodes.insert(std::make_pair(nid, Node<TNodeData>(nid)));
      return nid;
    }

    int add_node (TNodeData const& data) {
      int nid = ++max_node_id;
      nodes.insert(std::make_pair(nid, Node<TNodeData>(nid, data)));
      return nid;
    }

    /* Remove node and its edges */
    void remove_node (typename NodeMap::iterator nit) {
      for (std::list<int>::const_iterator eit = 
	     nit->second.in_edges.begin(); 
	   eit != nit->second.in_edges.end(); ++eit)
	remove_edge(*eit, true, false);
      for (std::list<int>::const_iterator eit = 
	     nit->second.out_edges.begin(); 
	   eit != nit->second.out_edges.end(); ++eit) 
	remove_edge(*eit, false, true);
      nodes.erase(nit);
    }

    /* Remove node and its edges */
    bool remove_node (int nid) {
      typename NodeMap::iterator nit = nodes.find(nid);
      remove_node(nit);
      return true;
    }

    int add_edge (int src_node, int tar_node, 
		  typename EdgeMap::iterator& neit) {
      typename NodeMap::iterator sit = nodes.find(src_node);
      typename NodeMap::iterator tit = nodes.find(tar_node);
      if (sit == nodes.end() || tit == nodes.end()) 
	perr("Error: cannot add edge without source or target node...");
      int eid = ++max_edge_id;
      neit = (edges.insert
	      (std::make_pair
	       (eid, Edge<TEdgeData>(eid, src_node, tar_node)))).first;
      sit->second.out_edges.push_back(eid);
      tit->second.in_edges.push_back(eid);
      return eid;
    }

    int add_edge (int src_node, int tar_node) {
      typename EdgeMap::iterator neit;
      return add_edge(src_node, tar_node, neit);
    }

    int add_edge (int src_node, int tar_node, TEdgeData const& data) {
      typename NodeMap::iterator sit = nodes.find(src_node);
      typename NodeMap::iterator tit = nodes.find(tar_node);
      if (sit == nodes.end() || tit == nodes.end()) 
	perr("Error: cannot add edge without source or target node...");
      int eid = ++max_edge_id;
      edges.insert(std::make_pair(eid, 
				  Edge<TEdgeData>(eid, 
						  src_node, 
						  tar_node, data)));
      sit->second.out_edges.push_back(eid);
      tit->second.in_edges.push_back(eid);
      return eid;
    }

    /* If to modify src/tar node, specify a pointer */
    /* Otherwise they remain unchanged */
    void remove_edge (typename EdgeMap::iterator eit, 
		      Node<TNodeData>* pSrcNode, 
		      Node<TNodeData>* pTarNode) {
      if (pSrcNode != NULL) {
	std::list<int>::iterator oeit = pSrcNode->out_edges.begin();
	while (oeit != pSrcNode->out_edges.end())
	  if (*oeit == eit->second.id) 
	    oeit = pSrcNode->out_edges.erase(oeit);
	  else ++oeit;
      }
      if (pTarNode != NULL) {
	std::list<int>::iterator ieit = pTarNode->in_edges.begin();
	while (ieit != pTarNode->in_edges.end()) 
	  if (*ieit == eit->second.id) 
	    ieit = pTarNode->in_edges.erase(ieit);
	  else ++ieit;
      }
      edges.erase(eit);
    }

    bool remove_edge (int eid, bool changeSrcNode, bool changeTarNode) {
      typename EdgeMap::iterator eit = edges.find(eid);
      if (eit == edges.end()) return false;
      Node<TNodeData>* pSrcNode = changeSrcNode? 
	&(nodes.find(eit->second.src_node)->second): NULL;
      Node<TNodeData>* pTarNode = changeTarNode? 
	&(nodes.find(eit->second.tar_node)->second): NULL;
      remove_edge(eit, pSrcNode, pTarNode);
      return true;
    }

    /* Return {(node_const_iterator, terminal_type)} */
    /* Type: 0 unknown, 1: no in edge, 2 no out edge, 3 no both */
    void get_terminals 
      (std::list<std::pair<typename NodeMap::const_iterator, int> >& 
       terminals) const {
      for (typename NodeMap::const_iterator it = nodes.begin(); 
	   it != nodes.end(); ++it) {
	if (it->second.in_edges.size() == 0 && 
	    it->second.out_edges.size() == 0) 
	  terminals.push_back(std::make_pair(it, 3));
	else if (it->second.in_edges.size() == 0) 
	  terminals.push_back(std::make_pair(it, 1));
	else if (it->second.out_edges.size() == 0) 
	  terminals.push_back(std::make_pair(it, 2));
      }
    }

    /* Return {(node_iterator, terminal_type)} */
    /* Type: 0 unknown, 1: no in edge, 2 no out edge, 3 no both */
    void get_terminals 
      (std::list<std::pair<typename NodeMap::iterator, int> >& terminals) {
      for (typename NodeMap::iterator it = nodes.begin(); 
	   it != nodes.end(); ++it) {
	if (it->second.in_edges.size() == 0 && 
	    it->second.out_edges.size() == 0) 
	  terminals.push_back(std::make_pair(it, 3));
	else if (it->second.in_edges.size() == 0) 
	  terminals.push_back(std::make_pair(it, 1));
	else if (it->second.out_edges.size() == 0) 
	  terminals.push_back(std::make_pair(it, 2));
      }
    }

    /* Return {(node_id, terminal_type)} */
    /* Type: 0 unknown, 1: no in edge, 2 no out edge, 3 no both */
    void get_terminals (std::list<std::pair<int, int> >& terminals) const {
      for (typename NodeMap::const_iterator it = nodes.begin(); 
	   it != nodes.end(); ++it) {
	if (it->second.in_edges.size() == 0 && 
	    it->second.out_edges.size() == 0) 
	  terminals.push_back(std::make_pair(it->second.id, 3));
	else if (it->second.in_edges.size() == 0) 
	  terminals.push_back(std::make_pair(it->second.id, 1));
	else if (it->second.out_edges.size() == 0) 
	  terminals.push_back(std::make_pair(it->second.id, 2));
      }
    }

    void get_nonterminals (std::list<typename NodeMap::const_iterator>& nt)
      const {
      for (typename NodeMap::const_iterator it = nodes.begin(); 
	   it != nodes.end(); ++it) 
	if (it->second.in_edges.size() != 0 && 
	    it->second.out_edges.size() != 0)
	  nt.push_back(it);
    }

    void get_nonterminals (std::list<typename NodeMap::iterator>& nt)
      const {
      for (typename NodeMap::iterator it = nodes.begin(); 
	   it != nodes.end(); ++it) 
	if (it->second.in_edges.size() != 0 && 
	    it->second.out_edges.size() != 0)
	  nt.push_back(it);
    }

    void get_nonterminals (std::list<int>& nt) const {
      for (typename NodeMap::const_iterator it = nodes.begin(); 
	   it != nodes.end(); ++it) 
	if (it->second.in_edges.size() != 0 && 
	    it->second.out_edges.size() != 0)
	  nt.push_back(it->second.id);
    }

    /* item: all, node, edge */
    void print (std::ostream& os, const char* item)
    {
      bool isAll = strcmp(item, "all") == 0;      
      if (isAll || strcmp(item, "basic") == 0)
	os << "id = " << id << "\n"
	   << "maxNodeID = " << max_node_id << "\n"
	   << "maxEdgeID = " << max_edge_id << "\n";
      if (isAll || strcmp(item, "node") == 0) {
	if (isAll) os << "\nnodes:\n";
	for (typename NodeMap::const_iterator nit = nodes.begin(); 
	     nit != nodes.end(); ++nit) {
	  if (isAll) {
	    os << nit->second.id << ":\nin_edges: ";
	    for (std::list<int>::const_iterator eit = 
		   nit->second.in_edges.begin(); 
		 eit != nit->second.in_edges.end(); ++eit) 
	      os << *eit << " ";
	    os << "\nout_edges: ";
	    for (std::list<int>::const_iterator eit = 
		   nit->second.out_edges.begin(); 
		 eit != nit->second.out_edges.end(); ++eit) 
	      os << *eit << " ";
	    os << "\ndata: ";
	  }
	  os << nit->second.data << "\n";
	}
      }
      if (isAll || strcmp(item, "edge") == 0) {
	if (isAll) os << "\nedges:\n";
	for (typename EdgeMap::const_iterator eit = edges.begin(); 
	     eit != edges.end(); ++eit) {
	  if (isAll) {
	    os << eit->second.id << ":\nsrc_node: " 
	       << eit->second.src_node << "\ntar_node: "
	       << eit->second.tar_node << "\ndata: ";
	  }
	  os << eit->second.data << "\n";
	}
      }
    }

    friend std::ostream& operator << (std::ostream& os, 
				      Graph const& graph) {
      os << graph.id << " " << graph.max_node_id << " " 
	 << graph.max_edge_id << " " << graph.nodes.size() << " ";
      for (typename Graph::NodeMap::const_iterator it = 
	     graph.nodes.begin(); it != graph.nodes.end(); ++it) {
	os << it->first << " ";
	os << it->second << " ";
      }
      os << graph.edges.size() << " ";
      for (typename Graph::EdgeMap::const_iterator it = 
	     graph.edges.begin(); it != graph.edges.end(); ++it) {
	os << it->first << " ";
	os << it->second << " ";
      }
      return os;
    }

    friend std::istream& operator >> (std::istream& is, Graph& graph) {
      int n;
      is >> graph.id >> graph.max_node_id >> graph.max_edge_id >> n;
      for (int i = 0; i < n; ++i) {
	int nid;
	is >> nid;
	typename NodeMap::iterator nit = 
	  graph.nodes.insert(graph.nodes.end(), 
			     std::make_pair(nid, Node<TNodeData>(-1)));
	is >> nit->second;
      }
      is >> n;
      for (int i = 0; i < n; ++i) {
	int eid;
	is >> eid;
	typename EdgeMap::iterator eit = 
	  graph.edges.insert(graph.edges.end(), 
			     std::make_pair(eid, Edge<TEdgeData>()));
	is >> eit->second;
      }
      return is;
    }

  }; 

};

#endif
