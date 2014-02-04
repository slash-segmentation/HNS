#ifndef _n3_tree3d_bc_h_
#define _n3_tree3d_bc_h_

#include "tree2d/bc.h"
#include "tree3d_util.h"

namespace n3 {

  namespace tree3d {

    /* Boundary classifier features:
       Consider: body 0 + body 1 -> body 2

       Unordered geometry: 
         touch border 0
	 touch border 1
         vol 0 (V0)
	 vol 1 (V1 >= V0)
	 vol diff (dV01 = |V1 - V0|)
	 vol diff ratio 0 (dV01 / V0)
	 vol diff ratio 1 (dV01 / V1)
	 surface area 0 (A0)
	 surface area 1 (A1)
	 surface area diff (dA01 = |A0 - A1|)
	 surface area diff ratio 0 (dA01 / A0)
	 surface area diff ratio 1 (dA01 / A1)
	 boundary surface area (B)
	 boundary surface area ratio 0 (B / A0)
	 boundary surface area ratio 1 (B / A1)
	 compactness 0 (A0 ^ 1.5 / V0)
	 compactness 1 (A1 ^ 1.5 / V1)

       Raw intensity: 
         Boundary:
	   hist
	   min
	   max
	   mean
	   median
	   std dev
	 Region: [NOT USED]
	   hist L1 dist
	   hist X2 dist
	   min abs diff 
	   max abs diff 
	   mean abs diff 
	   median abs diff
	   std dev abs diff
	 Texton: [NOT USED]
	   texton hist L1 dist
	   texton hist X2 dist
	 
       Pb intensity:
         Boundary:
	   hist
	   min
	   max
	   mean
	   median
	   std dev
	 Region: [NOT USED]
	   hist L1 dist
	   hist X2 dist
	   min abs diff 
	   max abs diff 
	   mean abs diff 
	   median abs diff
	   std dev abs diff

       Saliency:
         sub saliency 0 (S0)
	 sub saliency 1 (S1)
	 sup saliency (S2)
	 saliency inc 0 (dS0 = S2 - S0)
	 saliency inc 1 (dS1 = S2 - S1)
	 saliency inc abs diff (|dS0 - dS1|)
    */

    void getBoundaryUnorderedGeometryFeatures 
      (flist& feat, Points3 const* r0, Points3 const* r1, 
       Points3 const* c0, Points3 const* c1, Points3 const* ub, 
       Points3 const* b0, Points3 const* b1, bool swap01);

    void getBoundaryIntensityFeatures (flist& feat, Points3 const& p, 
				       FloatImage3::Pointer valImage, 
				       float histLower, float histUpper, 
				       int histBin);

    void getBoundaryIntensityFeatures (flist& feat, Points3 const& p0, 
				       Points3 const& p1, 
				       FloatImage3::Pointer valImage, 
				       float histLower, float histUpper, 
				       int histBin);

    void getBoundaryTextonFeatures (flist& feat, Points3 const& p0, 
				    Points3 const& p1, 
				    FloatImage3::Pointer valImage, 
				    TextonDict const& tdict, 
				    LabelImage3::Pointer canvas);

    // Does not include saliency features
    void getBoundaryFeatures (flist& feat, Points3 const* r0, 
			      Points3 const* r1, Points3 const* uc0, 
			      Points3 const* uc1, Points3 const* b0, 
			      Points3 const* b1, 
			      LabelImage3::Pointer canvas, 
			      FloatImage3::Pointer rawImage, 
			      FloatImage3::Pointer pbImage);

  };

};

#endif
