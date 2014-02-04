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

  template <typename T> bool 
    compContainer (T const& c0, T const& c1) {
    if (c0.size() < c1.size()) return true;
    else if (c0.size() == c1.size()) {
      typename T::const_iterator cit0 = c0.begin(); 
      typename T::const_iterator cit1 = c1.begin();
      while (cit0 != c0.end()) {
	if (*cit0 < *cit1) return true;
	else if (*cit0 > *cit1) return false;
	++cit0;
	++cit1;
      }
    }
    return false;
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


  /* Determine whether two containers' elemenets are exactly identical */
  template <typename TContainer0, typename TContainer1> bool 
    is_equal (TContainer0 const& c0, TContainer1 const& c1)
    {
      if (c0.size() != c1.size()) return false;
      typename TContainer0::const_iterator it0 = c0.begin(); 
      typename TContainer0::const_iterator it1 = c1.begin(); 
      while (it0 != c0.end()) {
	if (*it0 != *it1) return false;
	++it0;
	++it1;
      } 
      return true;
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

  
  /* If from0 and from1 are sorted, and need to to be sorted  */
  /* Use sort = true */
  template <typename TIn, typename TOut> void 
    combine (TOut& to, TIn const& from0, TIn const& from1, bool unique, 
	     bool sort) 
  {
    if (sort) {
      to = from0.size() > from1.size()? from0: from1;
      TIn t = from0.size() <= from1.size()? from0: from1;
      merge(to, t, unique);
    }
    else combine(to, from0, from1, unique);
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


  /* /\* Simultaneously merge a number of containers *\/ */
  /* template <typename TIn, typename TOut> void */
  /*   merge (TOut& to, std::list<TIn*> const& froms, bool unique) */
  /* { */
  /*   std::list<std::pair<TIn*, typename TIn::iterator> > ps; */
  /*   for (std::list<TIn*>::const_iterator fit = froms.begin(); */
  /* 	 fit !=froms.end(); ++fit) { */
  /*     typename TIn::iterator it = (*fit)->begin(); */
  /*     if (it != (*fit)->end()) ps.push_back(std::make_pair(*fit, it)); */
  /*   } */
  /*   while (ps.size() > 1) { */
  /*     std::list<std::pair<TIn*, typename TIn::iterator> >::iterator */
  /* 	pp = ps.begin(); */
  /*     for (std::list<std::pair<TIn*, typename TIn::iterator> >::iterator */
  /* 	     pit = ps.begin(); pit != ps.end(); ++pit) */
  /* 	if (*(pit->second) < *(pp->second)) pp = pit; */
  /*     to.splice(to.end(), *(pp->first), pp->second); */
  /*     ++(pp->second); */
  /*     if (pp->second == pp->first->end()) ps.erase(pp); */
  /*   } */
  /*   if (!ps.empty()) to.splice(to.end(), *(ps.front().first)); */
  /*   if (unique) to.unique(); */
  /* } */


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


  /* Compute union of two containers (e.g. std::map) */
  /* Take sum of two values that share same key  */
  template <typename TContainer> void 
    add (TContainer& res, TContainer const& m0, TContainer const& m1)
    {
      typename TContainer::const_iterator it0 = m0.begin(); 
      typename TContainer::const_iterator it1 = m1.begin(); 
      while (it0 != m0.end() && it1 != m1.end())
	if (it0->first < it1->first) {
	  res.insert(res.end(), *it0);
	  ++it0;
	}
	else if (it0->first == it1->first) {
	  res.insert(res.end(), 
		     std::make_pair(it0->first, 
				    it0->second + it1->second));
	  ++it0;
	  ++it1;
	}
	else {
	  res.insert(res.end(), *it1);
	  ++it1;
	}
      while (it0 != m0.end()) {
	res.insert(res.end(), *it0);
	++it0;
      }
      while (it1 != m1.end()) {
	res.insert(res.end(), *it1);
	++it1;
      }
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
    print (TContainer const& data, char delim, std::ostream& os) 
    {
      for (typename TContainer::const_iterator it = data.begin(); 
	   it != data.end(); ++it) os << *it << delim;
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
