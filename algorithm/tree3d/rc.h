#ifndef _n3_tree3d_rc_h_
#define _n3_tree3d_rc_h_

#include "util/cv/cv_ma.h"
#include "bc.h"

namespace n3 {

  namespace tree3d {

    const unsigned int RC_SEGLEN_HIST_BIN = 5;
    const unsigned int RC_BRNCHORDER_HIST_BIN = 5;
    const unsigned int RC_BRNCHORDER_HIST_UPPER = 
      RC_BRNCHORDER_HIST_BIN - 1;
    const unsigned int RC_ANGLE_HIST_BIN = 6;
    const unsigned int RC_INT_HIST_BIN = 10;

    /* Region classifier features:
       Consider: one 3d body 

       Shape:
         whether touches image border [1]
	 volume (V) [2]
	 surface area (A) [3]
	 compactness (A ^ 1.5 / V) [4]
	 segment length min [5]
	 segment length max [6]
	 segment length mean [7]
	 segment length median [8]
	 segment length std dev [9]
	 segment length sum (L) [10]
	 mean profile area (V / L) [11]
	 normalized segment length hist [12 - 16]
	 branch point # [17]
	 branch order hist [18 - 22]
	 branch order max [23]
	 branch order mean [24]
	 branch order median [25]
	 branch order std dev [26]
	 branch min angle hist [27 - 32]
	 branch min angle min [33]
	 branch min angle max [34]
	 branch min angle mean [35]
	 branch min angle median [36]
	 branch min angle std dev [37]
	 branch max angle hist [38 - 43]
	 branch max angle min [44]
	 branch max angle max [45]
	 branch max angle mean [46]
	 branch max angle median [47]
	 branch max angle std dev [48]
	 
       Raw intensity:
         surface raw intensity histogram [49 - 58]
	 surface raw intensity min [59]
	 surface raw intensity max [60]
	 surface raw intensity mean [61]
	 surface raw intensity median [62]
	 surface raw intensity std dev [63]

       Pb intensity:
         surface raw intensity histogram [64 - 73]
	 surface raw intensity min [74]
	 surface raw intensity max [75]
	 surface raw intensity mean [76]
	 surface raw intensity median [77]
	 surface raw intensity std dev [78]
    */

    // Start from longest edge
    // Count branching orders incrementally
    void getBranchOrders (std::list<int>& orders, MaGraph const& mag);

    void getRegionShapeFeatures (flist& feat, Points3 const& body, 
				 Points3 const& surface, 
				 Points3 const* border, 
				 MaGraph const& mag);

    void getRegionShapeFeatures (flist& feat, Points3 const& body, 
				 Points3 const& surface, 
				 Points3 const* border, 
				 Box3 const* pBodyBox, 
				 LabelImage3::Pointer canvas, 
				 double limbThreshold);

    void getRegionIntensityFeatures (flist& feat, Points3 const& surface, 
				     FloatImage3::Pointer valImage);

  };

};

#endif
