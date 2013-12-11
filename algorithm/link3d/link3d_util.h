#ifndef _n3_link3d_util_h_
#define _n3_link3d_util_h_

#include "util/struct_util.h"
#include "link3d.h"

namespace n3 {

  namespace link3d {

    inline bool cmp (std::set<IntLabelPair> const& s0, 
		     std::set<IntLabelPair> const& s1)
    {
      return s0.size() > s1.size();
    }


    template <typename TContainer> void 
      link (std::map<int, LabelImage::Pointer>& outputImages, 
	    std::map<int, LabelImage::Pointer> const& inputImages, 
	    TContainer const& links)
      {
	std::list<std::set<IntLabelPair> > psets;
	for (typename TContainer::const_iterator lit = links.begin(); 
	     lit != links.end(); ++lit) {
	  IntLabelPair profile0 = lit->get_profile(0);
	  IntLabelPair profile1 = lit->get_profile(1);
	  std::list<std::set<IntLabelPair> >::iterator sit0 = psets.end();
	  std::list<std::set<IntLabelPair> >::iterator sit1 = psets.end();
	  for (std::list<std::set<IntLabelPair> >::iterator sit = 
		 psets.begin(); sit != psets.end(); ++sit) {
	    if (sit->count(profile0) > 0) sit0 = sit;
	    if (sit->count(profile1) > 0) sit1 = sit;
	    if (sit0 != psets.end() && sit1 != psets.end()) break;
	  }
	  if (sit0 == psets.end() && sit1 == psets.end()) {
	    std::set<IntLabelPair>  tset;
	    psets.push_back(tset);
	    psets.back().insert(profile0);
	    psets.back().insert(profile1);
	  }
	  else if (sit0 != psets.end() && sit1 == psets.end()) 
	    sit0->insert(profile1);
	  else if (sit0 == psets.end() && sit1 != psets.end()) 
	    sit1->insert(profile0);
	  else {
	    sit0->insert(profile0);
	    sit0->insert(profile1);
	    if (sit0 != sit1) {
	      sit0->insert(sit1->begin(), sit1->end());
	      psets.erase(sit1);
	    }
	  }
	}
	psets.sort(cmp);
	int n = inputImages.size();
	std::vector<int> sliceNos(n, -1);
	std::map<int, int> sliceNoIndices; 
	int j = 0;
	for (std::map<int, LabelImage::Pointer>::const_iterator it = 
	       inputImages.begin(); it != inputImages.end(); ++it) {
	  sliceNoIndices[it->first] = j;
	  sliceNos[j] = it->first;
	  ++j;
	}
	std::vector<PointMap> rmaps(n);
	std::vector<LabelImage::Pointer> tmpOutputImages(n);
#pragma omp parallel for 
	for (int i = 0; i < n; ++i) {
	  int sliceNo = sliceNos[i];
	  getPointMap(rmaps[i], inputImages.find(sliceNo)->second, false);
	  tmpOutputImages[i] = 
	    createImage<LabelImage>(inputImages.find(sliceNo)->second, 0);
	}
	for (int i = 0; i < n; ++i) 
	  outputImages[sliceNos[i]] = tmpOutputImages[i];
	Label labelToAssign = 1;
	for (std::list<std::set<IntLabelPair> >::const_iterator sit = 
	       psets.begin(); sit != psets.end(); ++sit) {
	  for (std::set<IntLabelPair>::const_iterator pit = sit->begin(); 
	       pit != sit->end(); ++pit) {
	    int sliceNo = pit->first;
	    setvs<LabelImage>(outputImages[sliceNo], 
			      rmaps[sliceNoIndices[sliceNo]][pit->second], 
			      labelToAssign);
	  }
	  ++labelToAssign;
	}
	/* Turn bg pixels with one CRCONN fg neighbor to fg */
	for (std::map<int, LabelImage::Pointer>::iterator iit = 
	       outputImages.begin(); iit != outputImages.end(); ++iit) {
	  Points bg;
	  getPoints(bg, iit->second, BGVAL);
	  for (Points::const_iterator pit = bg.begin(); pit != bg.end(); 
	       ++pit) {
	    std::set<Label> nb; 
	    getNeighbors<LabelImage>(nb, *pit, iit->second, CRCONN);
	    nb.erase(BGVAL);
	    if (nb.size() == 1) 
	      setv<LabelImage>(iit->second, pit->x, pit->y, *(nb.begin())); 
	  }
	}
      }



    template <typename TContainer> void 
      link (std::map<int, LabelImage::Pointer>& labelImages, 
	    TContainer const& links)
      {
	std::list<std::set<IntLabelPair> > psets;
	for (typename TContainer::const_iterator lit = links.begin(); 
	     lit != links.end(); ++lit) {
	  IntLabelPair profile0 = lit->get_profile(0);
	  IntLabelPair profile1 = lit->get_profile(1);
	  std::list<std::set<IntLabelPair> >::iterator sit0 = psets.end();
	  std::list<std::set<IntLabelPair> >::iterator sit1 = psets.end();
	  for (std::list<std::set<IntLabelPair> >::iterator sit = 
		 psets.begin(); sit != psets.end(); ++sit) {
	    if (sit->count(profile0) > 0) sit0 = sit;
	    if (sit->count(profile1) > 0) sit1 = sit;
	    if (sit0 != psets.end() && sit1 != psets.end()) break;
	  }
	  if (sit0 == psets.end() && sit1 == psets.end()) {
	    std::set<IntLabelPair>  tset;
	    psets.push_back(tset);
	    psets.back().insert(profile0);
	    psets.back().insert(profile1);
	  }
	  else if (sit0 != psets.end() && sit1 == psets.end()) 
	    sit0->insert(profile1);
	  else if (sit0 == psets.end() && sit1 != psets.end()) 
	    sit1->insert(profile0);
	  else {
	    sit0->insert(profile0);
	    sit0->insert(profile1);
	    if (sit0 != sit1) {
	      sit0->insert(sit1->begin(), sit1->end());
	      psets.erase(sit1);
	    }
	  }
	}
	psets.sort(cmp);
	Label labelToAssign = 1;
	/* labelMap[slice][label2] = label3 */
	std::map<int, std::map<Label, Label> > labelMap; 
	/* boost::unordered_map<int,  */
	/*   boost::unordered_map<Label, Label> > labelMap;  */
	for (std::list<std::set<IntLabelPair> >::const_iterator sit = 
	       psets.begin(); sit != psets.end(); ++sit) {
	  for (std::set<IntLabelPair>::const_iterator pit = sit->begin(); 
	       pit != sit->end(); ++pit)
	    labelMap[pit->first][pit->second] = labelToAssign;
	  ++labelToAssign;
	}
	for (std::map<int, LabelImage::Pointer>::iterator iit = 
	       labelImages.begin(); iit != labelImages.end(); ++iit) {
	  /* boost::unordered_map<Label, Label> const* pl =  */
	  /*   &(labelMap.find(iit->first)->second); */
	  std::map<Label, Label> const* pl = 
	    &(labelMap.find(iit->first)->second);
	  for (itk::ImageRegionIterator<LabelImage> 
		 it(iit->second, iit->second->GetLargestPossibleRegion()); 
	       !it.IsAtEnd(); ++it) {
	    Label l2 = it.Get();
	    if (l2 != BGVAL) it.Set(pl->find(l2)->second);
	  }
	}
	/* Turn bg pixels with one CRCONN fg neighbor to fg */
	for (std::map<int, LabelImage::Pointer>::iterator iit = 
	       labelImages.begin(); iit != labelImages.end(); ++iit) {
	  Points bg;
	  getPoints(bg, iit->second, BGVAL);
	  for (Points::const_iterator pit = bg.begin(); pit != bg.end(); 
	       ++pit) {
	    std::set<Label> nb; 
	    getNeighbors<LabelImage>(nb, *pit, iit->second, CRCONN);
	    nb.erase(BGVAL);
	    if (nb.size() == 1) 
	      setv<LabelImage>(iit->second, pit->x, pit->y, *(nb.begin())); 
	  }
	}
      }



    template <typename TContainer> void 
      linkBodies (std::map<int, LabelImage::Pointer>& outputImages, 
		  std::map<int, LabelImage::Pointer> const& inputImages, 
		  TContainer const& bodyPairs) 
      {
	/* boost::unordered_map<Label, Label> lmap; */
	std::map<Label, Label> lmap;
	for (std::map<int, LabelImage::Pointer>::const_iterator iit = 
	       inputImages.begin(); iit != inputImages.end(); ++iit) {
	  for (itk::ImageRegionConstIterator<LabelImage> 
		 it(iit->second, iit->second->GetLargestPossibleRegion()); 
	       !it.IsAtEnd(); ++it) {
	    Label l = it.Get();
	    lmap[it.Get()] = l;
	  }
	  LabelImage::Pointer oi = createImage<LabelImage>(iit->second, 0);
	}
	for (typename TContainer::const_iterator pit = bodyPairs.begin(); 
	     pit != bodyPairs.end(); ++pit) {
	  Label lfrom = pit->nodes[0].first, lto = pit->nodes[1].first;
	  if (lfrom < lto) std::swap(lfrom, lto);
	  lmap[lfrom] = lto; 
	}
	for (std::map<int, LabelImage::Pointer>::const_iterator iit = 
	       inputImages.begin(); iit != inputImages.end(); ++iit) {
	  LabelImage::Pointer oi = createImage<LabelImage>(iit->second, 0);
	  itk::ImageRegionConstIterator<LabelImage> 
	    it0(iit->second, iit->second->GetLargestPossibleRegion());
	  itk::ImageRegionIterator<LabelImage> 
	    it1(oi, oi->GetLargestPossibleRegion());
	  while (!it0.IsAtEnd()) {
	    it1.Set(lmap[it0.Get()]);
	    ++it0;
	    ++it1;
	  }
	  outputImages[iit->first] = oi;
	}
      }


    
    /* Check on im if any point's value == v */
    template <typename TImage> bool
      check (Points const& points, typename TImage::Pointer im, 
	     typename TImage::PixelType v)
      {
	for (Points::const_iterator it = points.begin(); 
	     it != points.end(); ++it) 
	  if (getv<TImage>(im, it->x, it->y) == v) return true;
	return false;
      }


    
    /* Assume a 2D region is always contained inside a 3D body */
    /* Because 3D bodies may be dilated */
    /* Consider skipping a slice if skip == true */
    template <typename T> void 
      getBodies (std::list<Body<T> >& bodies, 
		 std::map<int, PointMap> const& rmaps2, 
		 std::map<int, LabelImage::Pointer> const& labelImages2,
		 std::map<int, LabelImage::Pointer> const& labelImages3, 
		 bool skip) 
      {
	std::map<Label, Body<T>*> pbs;
	bool firstSlice = true;
	for (std::map<int, LabelImage::Pointer>::const_iterator iit2 = 
	       labelImages2.begin(); iit2 != labelImages2.end(); ++iit2) {
	  int slice = iit2->first, prevSlice;
	  std::map<int, LabelImage::Pointer>::const_iterator iit3 = 
	    labelImages3.find(slice);
	  LabelImage::Pointer i2 = iit2->second;
	  LabelImage::Pointer i3 = iit3->second;
	  PointMap const* pr = &(rmaps2.find(slice)->second);
	  for (PointMap::const_iterator rit = pr->begin(); 
	       rit != pr->end(); ++rit) {
	    Label l2 = rit->first;
	    Label l3 = getv<LabelImage>(i3, rit->second.front().x, 
					rit->second.front().y);
	    typename std::map<Label, Body<T>*>::iterator bit = pbs.find(l3);
	    Body<T>* pb;
	    if (bit == pbs.end()) {
	      /* Never had body with l3 before or on first slice */
	      /* No need to search on previous slice for connections */
	      bodies.push_back(Body<T>(l3));
	      pb = &(bodies.back());
	      pbs[l3] = pb;
	      BodyNodeData nd(slice, l2, l3);
	      pb->add_node(nd);
	    }
	    else {
	      /* Otherwise grow from previous slice regions */
	      /* With same 3d label */
	      /* Unless it is first slice */
	      pb = bit->second;
	      std::list<int> from_ids;
	      if (!firstSlice) { /* Only when it is not first slice */
		std::map<int, LabelImage::Pointer>::const_iterator iit2p = 
		  prev(iit2); 
		std::map<int, LabelImage::Pointer>::const_iterator iit3p = 
		  prev(iit3); 
		prevSlice = iit2p->first;
		LabelImage::Pointer i2p = iit2p->second;
		LabelImage::Pointer i3p = iit3p->second;
		std::set<Label> l2ps; /* Overlapping 2d labels */
		for (Points::const_iterator pit = rit->second.begin(); 
		     pit != rit->second.end(); ++pit) {
		  Label l2p = getv<LabelImage>(i2p, pit->x, pit->y);
		  if (l2p != BGVAL && 
		      getv<LabelImage>(i3p, pit->x, pit->y) == l3)
		    l2ps.insert(l2p);
		}
		if (l2ps.size() != 0) {
		  /* Connected with regions one-slice below */
		  for (std::set<Label>::const_iterator lit = l2ps.begin(); 
		       lit != l2ps.end(); ++lit)
		    from_ids.push_back(pb->slmap
				       .find(prevSlice)->second
				       .find(*lit)->second);
		}
		else if (skip && iit2p != labelImages2.begin()) {
		  /* No direct connection; try skipping one slice */
		  std::map<int, LabelImage::Pointer>::const_iterator 
		    iit2p2 = prev(iit2p);
		  std::map<int, LabelImage::Pointer>::const_iterator 
		    iit3p2 = prev(iit3p);
		  LabelImage::Pointer i2p2 = iit2p2->second;
		  LabelImage::Pointer i3p2 = iit3p2->second;
		  std::set<Label> l2p2s; /* Overlapping 2d labels */
		  for (Points::const_iterator pit = rit->second.begin(); 
		       pit != rit->second.end(); ++pit) {
		    Label l2p2 = getv<LabelImage>(i2p2, pit->x, pit->y);
		    if (l2p2 != BGVAL && 
			getv<LabelImage>(i3p2, pit->x, pit->y) == l3)
		      l2p2s.insert(l2p2);
		  }
		  if (l2p2s.size() != 0) {
		    /* Connected with regions two-slice below */
		    for (std::set<Label>::const_iterator lit = 
			   l2p2s.begin(); lit != l2p2s.end(); ++lit)
		      from_ids.push_back(pb->slmap
					 .find(iit2p2->first)->second
					 .find(*lit)->second);
		  }
		}
	      }
	      BodyNodeData nd(slice, l2, l3);
	      if (from_ids.size() != 0)
		/* Connected with something below */
		/* Add node and arc(s) */
		pb->grow(from_ids, nd, true);
	      else
		/* Not connected with anything below */
		/* Add node without any arcs */
		pb->add_node(nd);
	    }
	  }
	  firstSlice = false;
	}
      }
 
  };

};

#endif
