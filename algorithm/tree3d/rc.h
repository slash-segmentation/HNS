#ifndef _n3_tree3d_rc_h_
#define _n3_tree3d_rc_h_

#include "util/cv/cv_ma.h"
#include "bc.h"

namespace n3 {

  namespace tree3d {

    const unsigned int RC_SEGLEN_HIST_BIN = 5;
    const unsigned int RC_BRNCHORDER_HIST_BIN = 5;
    const unsigned int RC_ANGLE_HIST_BIN = 6;
    const unsigned int RC_INT_HIST_BIN = 10;

    /* Region classifier features:
       Consider: one 3d body 

       Shape:
         whether touches image border
	 volume (V)
	 surface area (A)
	 compactness (A ^ 1.5 / V)
	 segment length min
	 segment length max
	 segment length mean
	 segment length median
	 segment length std dev
	 segment length sum (L)
	 mean profile area (V / L)
	 normalized segment length hist
	 branch point #
	 branch order hist
	 branch order max
	 branch order mean
	 branch order median
	 branch order std
	 branch min angle hist
	 branch min angle min
	 branch min angle max
	 branch min angle mean
	 branch min angle median
	 branch min angle std dev
	 branch max angle hist
	 branch max angle min
	 branch max angle max
	 branch max angle mean
	 branch max angle median
	 branch max angle std dev
	 
       Raw intensity:
         surface raw intensity histogram
	 surface raw intensity min
	 surface raw intensity max 
	 surface raw intensity mean
	 surface raw intensity median
	 surface raw intensity std dev
    */

    void getRegionShapeFeatures (flist& feat, Points3 const& body, 
				 Points3 const& surface, 
				 Box3 const& bodyBox, 
				 LabelImage3::Pointer canvas, 
				 bool touchBorder);

    void getRegionIntensityFeatures (flist& feat, Points3 const& surface, 
				     FloatImage3::Pointer valImage);

  };

};

#endif
