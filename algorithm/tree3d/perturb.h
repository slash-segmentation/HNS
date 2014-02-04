#ifndef _n3_tree3d_perturb_h_
#define _n3_tree3d_perturb_h_
#include "tree3d_util.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/discrete_distribution.hpp>

namespace n3 {

  namespace tree3d {

    // Metric: 1: min, 2: mean, 3: median
    void getMerges (std::list<tree2d::fMerge>& merges, Points3 const& bp, 
		    LabelImage3::Pointer labelImage, 
		    FloatImage3::Pointer valImage, 
		    int metric, const long seed);
    
  };

};

#endif
