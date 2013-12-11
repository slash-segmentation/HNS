#ifndef _n3_tree2d_perturb_h_
#define _n3_tree2d_perturb_h_

#include "tree2d_util.h"

namespace n3 {

  namespace tree2d {

    void getMerges (std::list<fMerge>& merges, 
		    LabelImage::Pointer labelImage, 
		    FloatImage::Pointer valImage, 
		    int metric, const long seed);
    
  };

};

#endif
