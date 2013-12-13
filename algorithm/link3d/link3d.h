#ifndef _n3_link3d_h_
#define _n3_link3d_h_

#include "struct/point.h"
#include "struct/graph.h"

namespace n3 {

  namespace link3d {

    template <typename T> struct Link { /* Between 2D regions */
      
      int s0, s1; /* Slice numbers */
      Label r0, r1; /* 2D region labels */
      T data;

      Link (int s0 = -1, Label r0 = 0, int s1 = -1, Label r1 = 0) 
      : s0(s0), s1(s1), r0(r0), r1(r1) {}

      Link (int s0, Label r0, int s1, Label r1, T const& data) 
      : s0(s0), s1(s1), r0(r0), r1(r1), data(data) {}
    
      LabelPair get_region_pair () const {return std::make_pair(r0, r1);}
      
      IntLabelPair get_profile (int i) const {
	return i == 0? std::make_pair(s0, r0): std::make_pair(s1, r1);
      }

      friend std::ostream& operator << 
      (std::ostream& os, Link<T> const& link) {
	os << "(" << link.s0 << ", " << link.r0 << ") -- (" 
	   << link.s1 << ", " << link.r1 << "): " << link.data; 
	return os;
      }

      bool operator < (Link const& l) const {
	return (s0 < l.s0) || (s0 == l.s0 && s1 < l.s1) || 
	  (s0 == l.s0 && s1 == l.s1 && r0 < l.r0) || 
	  (s0 == l.s0 && s1 == l.s1 && r0 == l.r0 && r1 < l.r1);
      }

    };


    typedef Link<double> fLink;


    /* Body node data */
    struct BodyNodeData {

      int slice;
      Label label2, label3;
      int status;

      BodyNodeData (int slice = -1, Label label2 = 0, 
		    Label label3 = 0, int status = 0) 
      : slice(slice), label2(label2), label3(label3), status(status) {}

      bool operator < (BodyNodeData const& nd) {
	return slice < nd.slice || 
	  (slice == nd.slice && (label2 < nd.label2 || 
				 label2 == nd.label2 && 
				 label3 < nd.label3));
      }

      friend std::ostream& operator << 
      (std::ostream& os, BodyNodeData const& nd) {
	os << nd.slice << " " << nd.label2 << " " << nd.label3 << " " 
	   << nd.status;
	return os;
      }

      friend std::istream& operator >> 
      (std::istream& is, BodyNodeData& nd) {
	is >> nd.slice >> nd.label2 >> nd.label3 >> nd.status;
	return is;
      }

    };


    /* Body */
    template <typename TBodyData> class Body 
      : public Graph<BodyNodeData, double> {
      
    public:
      /* typedef boost::unordered_map<Label, int> uiHashTable; */
      /* typedef boost::unordered_map<int, uiHashTable> iuiHashTable; */
      typedef std::map<Label, int> uiHashTable;
      typedef std::map<int, uiHashTable> iuiHashTable;
      typedef Node<BodyNodeData> Node;
      typedef Edge<double> Edge;
      
      Label label;
      int status;
      /* slmap[slice][label2] <- node_id */
      iuiHashTable slmap;
      TBodyData data;


      Body (Label label = 0, int status = 0) 
	: Graph<BodyNodeData, double>(-1), label(label), status(status) {}


      Body (Label label, int status, TBodyData const& data)
	: Graph<BodyNodeData, double>(-1), label(label), status(status), data(data) {}


      /* Add a node (profile) */
      int add_node (BodyNodeData const& nd) {
	int nid = Graph<BodyNodeData, double>::add_node(nd);
	slmap[nd.slice].insert(std::make_pair(nd.label2, nid));
	return nid;
      }


      /* Grow a node from nodes of from_ids  */
      /* Use node from_id as source (up == true) or target (otherwise) */
      void grow (std::list<int> const& from_ids, BodyNodeData const& nd, 
		 bool up) {
	int to_id = add_node(nd);
	if (up)
	  for (std::list<int>::const_iterator it = from_ids.begin(); 
	       it != from_ids.end(); ++it) add_edge(*it, to_id);
	else 
	  for (std::list<int>::const_iterator it = from_ids.begin(); 
	       it != from_ids.end(); ++it) add_edge(to_id, *it);
      }


      /* Keep terminals on top/bottom slice if includeTopBottom == true */
      /* If includeTopBottom == true, topSlice/bottomSlice are not used */
      void get_terminals (std::list<IntPair>& terminals, 
			  bool includeTopBottom, int bottomSlice = -1, 
			  int topSlice = -1) const {
	if (includeTopBottom) Graph<BodyNodeData, double>::get_terminals(terminals);
	else 
	  for (NodeMap::const_iterator it = nodes.begin(); 
	       it != nodes.end(); ++it)
	    if (it->second.data.slice != topSlice && 
		it->second.data.slice != bottomSlice) {
	      if (it->second.in_edges.size() == 0 && 
		  it->second.out_edges.size() == 0)
		terminals.push_back(std::make_pair(it->second.id, 3));
	      else if (it->second.in_edges.size() == 0)
		terminals.push_back(std::make_pair(it->second.id, 1));
	      else if (it->second.out_edges.size() == 0) 
		terminals.push_back(std::make_pair(it->second.id, 2));
	    }
      }


      friend std::ostream& operator << (std::ostream& os, Body const& b) {
	os << b.label << " " << b.status << " " << b.slmap.size() << " ";
	for (iuiHashTable::const_iterator hit0 = b.slmap.begin(); 
	     hit0 != b.slmap.end(); ++hit0) {
	  os << hit0->first << " " << hit0->second.size() << " ";
	  for (uiHashTable::const_iterator hit1 = hit0->second.begin(); 
	       hit1 != hit0->second.end(); ++hit1)
	    os << hit1->first << " " << hit1->second << " ";
	}
	os << static_cast<Graph<BodyNodeData, double> const&>(b);
	return os;
      }


      friend std::istream& operator >> (std::istream& is, Body& b) {
	int n;
	is >> b.label >> b.status >> n;
	for (int i = 0; i < n; ++i) {
	  int slice, nn;
	  is >> slice >> nn;
	  uiHashTable* p = &(b.slmap[slice]);
	  for (int j = 0; j < nn; ++j) {
	    Label l2;
	    int nid;
	    is >> l2 >> nid;
	    p->insert(std::make_pair(l2, nid));
	  }
	}
	is >> static_cast<Graph<BodyNodeData, double>&>(b);
	return is;
      }

    };



    /* Link between two bodies */
    template <typename T> struct BodyLink {
      /* Directed from node[0] to node[1] */
      std::pair<Label, int> nodes[2]; /* (label3, node_id) */
      T data;

      BodyLink (Label l0 = 0, int id0 = -1, Label l1 = 0, int id1 = -1) {
	nodes[0].first = l0;
	nodes[0].second = id0;
	nodes[1].first = l1;
	nodes[1].second = id1;
      }

      BodyLink (Label l0, int id0, Label l1, int id1, T const& data) {
	nodes[0].first = l0;
	nodes[0].second = id0;
	nodes[1].first = l1;
	nodes[1].second = id1;
	this->data = data;
      }

      bool operator < (BodyLink<T> const& l) {
	return nodes[0].first < l.nodes[0].first || 
	  (nodes[0].first == l.nodes[0].first && 
	   nodes[0].second < l.nodes[0].second) || 
	  (nodes[0].first == l.nodes[0].first && 
	   nodes[0].second == l.nodes[0].second && 
	   nodes[1].first < l.nodes[1].first) || 
	  (nodes[0].first == l.nodes[0].first && 
	   nodes[0].second == l.nodes[0].second && 
	   nodes[1].first == l.nodes[1].first && 
	   nodes[1].second < l.nodes[2].second);
      }

      friend std::ostream& operator << (std::ostream& os, 
					BodyLink const& l) {
	os << l.nodes[0].first << " " << l.nodes[0].second << " "
	   << l.nodes[1].first << " " << l.nodes[1].second;
	return os;
      }

      friend std::istream& operator >> (std::istream& is, BodyLink& l) {
	is >> l.nodes[0].first >> l.nodes[0].second 
	   >> l.nodes[1].first >> l.nodes[1].second;
	return is;
      }

    };


    typedef Body<double> fBody;
    typedef BodyLink<double> fBodyLink;

  };

};

#endif
