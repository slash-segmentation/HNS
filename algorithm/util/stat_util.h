#ifndef _n3_stat_util_h_
#define _n3_stat_util_h_

#include "n3_util.h"

namespace n3 {

  template <typename TContainer> double 
    getSum (TContainer const& data) 
    {
      double ret = 0.0;
      for (typename TContainer::const_iterator it = data.begin(); 
	   it != data.end(); ++it) ret += (double)*it;
      return ret;
    }



  /* If sum approaches 0, do not normalize */
  template <typename TContainer> void 
    normalize (TContainer& data)
    {
      double s = getSum(data);
      if (fabs(s) < EPSILON) return;
      for (typename TContainer::const_iterator it = data.begin(); 
	   it != data.end(); ++it) 
	*it = (TContainer::value_type)((double)*it / s);
    }



  template <typename TContainer> double
    getMean (TContainer const& data) 
    {
      return getr(getSum(data), data.size());
    }



  template <typename TContainer> double 
    getStd (TContainer const& data, typename TContainer::value_type& mean)
    {
      if (data.size() == 0) return DUMMY;
      double s = 0.0;
      for (typename TContainer::const_iterator it = data.begin(); 
	   it != data.end(); ++it) s += (*it - mean) * (*it - mean);
      return sqrt(getr(s, data.size()));
    }



  template <typename TContainer> typename TContainer::value_type
    getMin (TContainer const& data)
    {
      if (data.size() == 0) return DUMMY;
      typename TContainer::value_type ret = data.front();
      for (typename TContainer::const_iterator it = data.begin(); 
	   it != data.end(); ++it) if (*it < ret) ret = *it;
      return ret;
    }



  template <typename TContainer> typename TContainer::value_type
    getMax (TContainer const& data)
    {
      if (data.size() == 0) return DUMMY;
      typename TContainer::value_type ret = data.front();
      for (typename TContainer::const_iterator it = data.begin(); 
	   it != data.end(); ++it) if (*it > ret) ret = *it;
      return ret;
    }



  template <typename TVal> double
    getMedian (std::list<TVal>& data, bool isSorted = false)
    {
      if (data.size() == 0) return DUMMY;
      if (!isSorted) data.sort();
      int len = data.size();
      typename std::list<TVal>::const_iterator it = data.begin();
      if (len % 2 != 0) {
  	std::advance(it, len / 2);
  	return (double)*it;
      }
      else {
  	std::advance(it, len / 2);
  	TVal r = *it;
  	--it;
  	return (double)(*it + r) / 2.0;
      }
    }



  template <typename TVal> double 
    getMedian (std::vector<TVal>& data)
    {
      if (data.size() == 0) return DUMMY;
      std::sort(data.begin(), data.end());
      int len = data.size();
      return len % 2 == 0? (double)data[len / 2]: 
	((double)data[len / 2 - 1] + (double)data[len / 2]) / 2.0;
    }



  template <typename T> fvec 
    getHist (T const& data, typename T::value_type lower, 
	     typename T::value_type upper, int bin, bool normalize = true) 
    {
      fvec hist(bin, 0.0);
      if (data.size() > 0) {
	double intrv = (double)(upper - lower) / (double)bin;
	for (typename T::const_iterator it = data.begin(); 
	     it != data.end(); ++it) {
	  if (*it <= lower) hist[0] += 1.0;
	  else if (*it >= upper) hist[bin - 1] += 1.0;
	  else
	    for (int i = 0; i < bin; ++i) 
	      if ((double)*it <= (double)lower + intrv * (double)(i + 1)) {
		hist[i] += 1.0;
		break;
	      }
	}
	if (normalize)
	  for (fvec::iterator it = hist.begin(); it != hist.end(); ++it) 
	    *it = (float)((double)*it / (double)data.size());
      }
      return hist;
    }



  template <typename TContainerIn, typename TContainerOut> void
    getDeriv (TContainerOut& deriv, TContainerIn const& data)
    {
      if (data.size() < 2) return;
      typename TContainerIn::const_iterator it0 = data.begin();
      typename TContainerIn::const_iterator it1 = it0; ++it1;
      while (it1 != data.end()) {
	deriv.insert(deriv.end(), *it1 - *it0);
	++it0;
	++it1;
      }
    }

};

#endif
