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
	os << node.data;
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
	   << edge.tar_node << " " << edge.data;
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

    int add_edge (int src_node, int tar_node) {
      typename NodeMap::iterator sit = nodes.find(src_node);
      typename NodeMap::iterator tit = nodes.find(tar_node);
      if (sit == nodes.end() || tit == nodes.end()) 
	perr("Error: cannot add edge without source or target node...");
      int eid = ++max_edge_id;
      edges.insert(std::make_pair(eid, 
				  Edge<TEdgeData>(eid, 
						  src_node, tar_node)));
      sit->second.out_edges.push_back(eid);
      tit->second.in_edges.push_back(eid);
      return eid;
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

    /* Return {(node_id, terminal_type)} */
    /* Terminal type: 0 unknown, 1: no in edge, 2 no out edge, 3 none */
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

    void get_nonterminals (std::list<int>& nt) const {
      for (typename NodeMap::const_iterator it = nodes.begin(); 
	   it != nodes.end(); ++it) {
	if (it->second.in_edges.size() != 0 && 
	    it->second.out_edges.size() != 0)
	  nt.push_back(it->second.id);
      }
    }

    void print (bool std_out) {
      if (std_out) {
	std::cout << "id = " << id << "\n"
		  << "maxNodeID = " << max_node_id << "\n"
		  << "maxEdgeID = " << max_edge_id << "\n"
		  << "NodeMap: ";
	for (typename NodeMap::const_iterator nit = nodes.begin(); 
	     nit != nodes.end(); ++nit)
	  std::cout << "(" << nit->first << ": " 
		    << nit->second.data << ")\n";
	std::cout << "EdgeMap: ";
	for (typename EdgeMap::const_iterator eit = edges.begin(); 
	     eit != edges.end(); ++eit)
	  std::cout << "(" << eit->first << " (" << eit->second.src_node 
		    << " " << eit->second.tar_node << ")" << ": " 
		    << eit->second.data << ")\n";
      }
      else {
	std::cerr << "id = " << id << "\n"
		  << "maxNodeID = " << max_node_id << "\n"
		  << "maxEdgeID = " << max_edge_id << "\n"
		  << "NodeMap: ";
	for (typename NodeMap::const_iterator nit = nodes.begin(); 
	     nit != nodes.end(); ++nit)
	  std::cerr << "(" << nit->first << ": " 
		    << nit->second.data << ")\n";
	std::cerr << "EdgeMap: ";
	for (typename EdgeMap::const_iterator eit = edges.begin(); 
	     eit != edges.end(); ++eit)
	  std::cerr << "(" << eit->first << " (" << eit->second.src_node 
		    << " " << eit->second.tar_node << ")" << ": " 
		    << eit->second.data << ")\n";
      }
    }

    friend std::ostream& operator << (std::ostream& os, 
				      Graph const& graph) {
      os << graph.id << " " << graph.max_node_id << " " 
	 << graph.max_edge_id << " " << graph.nodes.size() << " ";
      for (typename Graph::NodeMap::const_iterator it = 
	     graph.nodes.begin(); it != graph.nodes.end(); ++it) 
	os << it->second << " ";
      os << graph.edges.size() << " ";
      for (typename Graph::EdgeMap::const_iterator it = 
	     graph.edges.begin(); it != graph.edges.end(); ++it) 
	os << it->second << " ";
      return os;
    }

    friend std::istream& operator >> (std::istream& is, Graph& graph) {
      int n;
      is >> graph.id >> graph.max_node_id >> graph.max_edge_id >> n;
      for (int i = 0; i < n; ++i) {
	Node<TNodeData> node(-1);
	is >> node;
	graph.nodes.insert(std::make_pair(node.id, node));
      }
      is >> n;
      for (int i = 0; i < n; ++i) {
	Edge<TEdgeData> edge;
	is >> edge;
	graph.edges.insert(std::make_pair(edge.id, edge));
      }
      return is;
    }

  }; 

};

#endif
