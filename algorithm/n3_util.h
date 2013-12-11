#ifndef _n3_util_h_
#define _n3_util_h_

#include "n3_base.h"

namespace n3 {

  template <typename TVal0, typename TVal1> double 
    getr (TVal0 const& v0, TVal1 const& v1) 
  {
    return fabs((double)v1) < EPSILON? DUMMY: ((double)v0 / (double)v1);
  }


  inline void assignDouble (double& v0, double const& v1) {v0 = v1;}
  

  inline bool compDouble (double const& v0, double const& v1) {
    return v0 < v1;
  }


  /* Return total size of x, which is a container of container */
  template <typename T> unsigned int
    size (T const& x) 
    {
      unsigned int ret = 0;
      for (typename T::const_iterator it = x.begin(); it != x.end(); ++it) 
	ret += it->size();
      return ret;
    }

  
  /* Remove all iterators in r from m */
  template <typename T> void 
    remove (T& m, std::list<typename T::iterator> const& r)
    {
      for (typename std::list<typename T::iterator>::const_iterator 
	     it = r.begin(); it != r.end(); ++it) m.erase(*it);
    }


  /* Remove elements in r from m */
  /* If m and r are both sorted in same way, set sort to true for speed */
  /* If to remove duplicate elements in m, set multiple to true */
  template <typename TIn, typename TOut> void 
    remove (TOut& m, TIn const& r, bool multiple, bool sort)
  {
    if (sort) {
      typename TOut::iterator mit = m.begin(); 
      for (typename TIn::const_iterator rit = r.begin(); rit != r.end(); 
	   ++rit) {
	while (mit != m.end() && *mit != *rit) ++mit;
	if (mit == m.end()) return;
	mit = m.erase(mit);
	if (multiple) while (mit != m.end() && *mit == *rit) m.erase(mit);
      }
    }
    else
      for (typename TIn::const_iterator rit = r.begin(); rit != r.end(); 
	   ++rit) 
	for (typename TOut::iterator mit = m.begin(); mit != m.end(); 
	     ++mit) 
	  if (*rit == *mit) {
	    mit = m.erase(mit);
	    --mit;
	    if (!multiple) break;
	  }
  }


  template <typename TIn, typename TOut> void 
    append (TOut& to, TIn const& from)
  {
    for (typename TIn::const_iterator it = from.begin(); 
	 it != from.end(); ++it) to.insert(to.end(), *it);
  }


  template <typename TIn, typename TOut> void 
    append (TOut& to, TIn const& from, bool unique) 
  {
    append(to, from);
    if (unique) {
      to.sort();
      to.unique();
    }
  }


  template <typename TIn, typename TOut> void 
    combine (TOut& to, TIn const& from0, TIn const& from1)
  {
    to = from0;
    append(to, from1);
  }


  template <typename TIn, typename TOut> void 
    combine (TOut& to, TIn const& from0, TIn const& from1, bool unique)
  {
    to = from0;
    append(to, from1, unique);
  }


  template <typename TIn, typename TOut> void 
    splice (TOut& to, TIn& from, bool unique)
  {
    to.splice(to.end(), from);
    if (unique) {
      to.sort();
      to.unique();
    }
  }


  template <typename TIn, typename TOut> void 
    merge (TOut& to, TIn& from, bool unique) 
  {
    to.merge(from);
    if (unique) to.unique();
  }


  template <typename TContainer> TContainer 
    add (TContainer const& v0, TContainer const& v1)
    {
      if (v0.size() != v1.size()) 
	perr("Error: container dimensions disagree...");
      TContainer ret = v0;
      typename TContainer::iterator rit = ret.begin(); 
      typename TContainer::const_iterator it1 = v1.begin();
      while (rit != ret.end()) {
	*rit += *it1;
	++rit;
	++it1;
      }
      return ret;
    }



  template <typename TContainer> TContainer 
    subtract (TContainer const& v0, TContainer const& v1)
    {
      if (v0.size() != v1.size()) 
	perr("Error: container dimensions disagree...");
      TContainer ret = v0;
      typename TContainer::iterator rit = ret.begin(); 
      typename TContainer::const_iterator it1 = v1.begin();
      while (rit != ret.end()) {
	*rit -= *it1;
	++rit;
	++it1;
      }
      return ret;
    }


  template <typename TContainer> TContainer 
    abs (TContainer const& data)
    {
      TContainer ret = data;
      for (typename TContainer::iterator it = ret.begin(); 
	   it != ret.end(); ++it) if (*it < 0) *it = -(*it);
      return ret;
    }



  template <typename TContainer> TContainer
    product (TContainer const& v0, TContainer const& v1)
    {
      if (v0.size() != v1.size()) 
	perr("Error: container dimensions disagree...");
      TContainer ret = v0;
      typename TContainer::iterator rit = ret.begin(); 
      typename TContainer::const_iterator it1 = v1.begin();
      while (rit != ret.end()) {
	*rit *= *it1;
	++rit;
	++it1;
      }
      return ret;
    }



  template <typename TContainer> TContainer
    product (TContainer const& v0, 
	     typename TContainer::value_type const& c)
    {
      TContainer ret = v0;
      for (typename TContainer::iterator rit = ret.begin(); 
	   rit != ret.end(); ++rit) *rit *= c;
      return ret;
    }


  
  /* If approx is true, measure floating point equality */
  template <typename TContainer> bool 
    any (TContainer const& data, typename TContainer::value_type val, 
	 bool approx = false)
    {
      if (approx) {
	for (typename TContainer::const_iterator it = data.begin(); 
	     it != data.end(); ++it) 
	  if (fabs((double)*it - (double)val) < EPSILON) return true;
      }
      else {
	for (typename TContainer::const_iterator it = data.begin(); 
	     it != data.end(); ++it) 
	  if (*it == val) return true;
      }
      return false;
    }



  /* If approx is true, measure floating point equality */
  template <typename TContainer> bool 
    all (TContainer const& data, typename TContainer::value_type val, 
	 bool approx = false)
    {
      if (approx) {
	for (typename TContainer::const_iterator it = data.begin(); 
	     it != data.end(); ++it) 
	  if (fabs((double)*it - (double)val) >= EPSILON) return false;
      }
      else {
	for (typename TContainer::const_iterator it = data.begin(); 
	     it != data.end(); ++it) 
	  if (*it != val) return false;
      }
      return true;
    }




  template <typename TContainer> double  
    norm (TContainer const& data, unsigned int order) 
    {
      if (order == 0) 
	if (any(data, 0, true)) perr("Error: cannot compute 0 ^ 0...");
	else return (double)data.size();
      double s = 0.0;
      for (typename TContainer::const_iterator it = data.begin(); 
	   it != data.end(); ++it) {
	double a = 1.0;
	for (int i = 0; i < order; ++i) a *= fabs((double)*it);
	s += a;
      }
      if (order == 1) return s;
      else if (order == 2) return sqrt(s);
      else return pow(s, 1.0 / (double)order);
    }




  template <typename TContainer> void 
    print (TContainer const& data, char delim = '\n', 
	   bool isStdOut = false) 
    {
      for (typename TContainer::const_iterator it = data.begin(); 
	   it != data.end(); ++it) 
	if (isStdOut) std::cout << *it << delim;
	else std::cerr << *it << delim;
    }


  /* No out of bound check! */
  template <typename TIterator> TIterator 
    next (TIterator it) {return ++it;}

  
  /* No out of bound check! */
  template <typename TIterator> TIterator
    prev (TIterator it) {return --it;}


  /* No out of bound check! */
  template <typename TContainer> typename TContainer::value_type 
    next (TContainer const& data, 
	  typename TContainer::value_type const& this_data)
    {
      typename TContainer::const_iterator it = data.find(this_data);
      if (it == data.end()) perr("Error: cannot find this data...");
      ++it;
      return *it;
    }


  /* No out of bound check! */
  template <typename TContainer> typename TContainer::value_type 
    prev (TContainer const& data, 
	  typename TContainer::value_type const& this_data)
    {
      typename TContainer::const_iterator it = data.find(this_data);
      if (it == data.end()) perr("Error: cannot find this data...");
      --it;
      return *it;
    }

};

#endif
