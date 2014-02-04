#ifndef _n3_tree2d_h_
#define _n3_tree2d_h_

#include "struct/bitree.h"

namespace n3 {

  namespace tree2d {

    template <typename T> struct Merge {

      Label from0, from1, to;
      T data;

      Merge (Label from0, Label from1, Label to, T const& data) 
      : from0(from0), from1(from1), to(to), data(data) {}

      Merge (Label from0, Label from1, Label to) 
      : from0(from0), from1(from1), to(to) {}
      
      bool operator < (Merge const& merge) const {return data < merge.data;}
      
      void print (std::ostream& os) {
	os << "from0 = " << from0 << ", from1 = " << from1 
	   << ", to = " << to << ", data = {" << data 
	   << "}";
      }

      friend std::ostream& operator << (std::ostream& os, 
					Merge<T> const& merge) {
	os << merge.from0 << " " << merge.from1 << " "
	   << merge.to << " " << merge.data << " ";
	return os;
      }

      friend std::istream& operator >> (std::istream& is, Merge<T>& merge) {
	is >> merge.from0 >> merge.from1 >> merge.to >> merge.data;
	return is;
      }

    };

    typedef Merge<double> fMerge;
    typedef BiTree<double> fTree;
  };

};

#endif
