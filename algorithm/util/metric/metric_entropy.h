#ifndef _n3_metric_entropy_h_
#define _n3_metric_entropy_h_

#include "util/image_util.h"

namespace n3 {

  /* Compute H(image1 | image0) */
  /* nPixel: number of pixels in one image */
  /* Base can be 0 (for 10), 1 for (e) or 2 (for 2) */
  template <typename TImage> double 
    getConditionalEntropy (typename TImage::Pointer image0, 
			   typename TImage::Pointer image1, 
			   int base, bool ignoreBG)
    {
      typedef typename TImage::PixelType TVal;
      double (*plog)(double) = NULL;
      switch (base) {
      case 0: 
	plog = &log10;
	break;
      case 1:
      case 2: 
	plog = &log;
	break;
      default: 
	perr("Error: unsupported entropy base...");
      }
      std::map<TVal, unsigned int> labels; // #x
      std::map<TVal, std::map<Label, unsigned int> > pairs; // #(x, y)
      unsigned int nPixel = 0;
      itk::ImageRegionConstIterator<TImage> 
	iit0(image0, image0->GetLargestPossibleRegion()), 
	iit1(image1, image1->GetLargestPossibleRegion());
      while (!iit0.IsAtEnd()) {
	Label l0 = iit0.Get(), l1 = iit1.Get();
	if ((ignoreBG && l0 != BGVAL && l1 != BGVAL) || !ignoreBG) {
	  ++nPixel;
	  typename std::map<TVal, unsigned int>::iterator lit = 
	    labels.find(l0);
	  if (lit == labels.end()) labels.insert(lit, std::make_pair(l0, 1));
	  else ++(lit->second);
	  typename std::map<TVal, std::map<TVal, unsigned int> >::iterator 
	    pit0 = pairs.find(l0);
	  if (pit0 == pairs.end()) pairs[l0][l1] = 1;
	  else {
	    typename std::map<TVal, unsigned int>::iterator pit1 = 
	      pit0->second.find(l1);
	    if (pit1 == pit0->second.end()) 
	      pit0->second.insert(pit1, std::make_pair(l1, 1));
	    else ++(pit1->second);
	  }
	}
	++iit0;
	++iit1;
      }
      double ret = 0.0;
      for (typename std::map<TVal, unsigned int>::const_iterator lit = 
	     labels.begin(); lit != labels.end(); ++lit) {
	typename std::map<TVal, std::map<TVal, unsigned int> >::
	  const_iterator pit0 = pairs.find(lit->first);
	if (pit0 != pairs.end()) {
	  for (typename std::map<TVal, unsigned int>::const_iterator pit1 = 
		 pit0->second.begin(); pit1 != pit0->second.end(); ++pit1)
	    ret += (double)pit1->second * 
	      (*plog)((double)lit->second / (double)pit1->second);
	}
      }
      if (base == 2) ret /= LN2;
      ret /= (double)nPixel;
      return ret;
    }

};

#endif
