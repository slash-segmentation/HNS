#ifndef _n3_tree2d_bc_h_
#define _n3_tree2d_bc_h_

#include "util/cv/cv_texton.h"
#include "util/cv/cv_hist.h"
#include "tree2d_util.h"

namespace n3 {

  namespace tree2d {

    /* Raw intensity histogram */
    const unsigned int BC_RI_HIST_BIN = 10;
    /* Probability histogram */
    const unsigned int BC_PB_HIST_BIN = 10;
    /* Saliency histogram */
    const unsigned int BC_SL_HIST_BIN = 10;
    /* Curve angle histogram */
    const unsigned int BC_CA_HIST_BIN = 6;
    /* Boundary intensity derivative histogram */
    const unsigned int BC_BD_HIST_BIN = 10;

    /* Boundary classifier features: 
       Consider: region 0 + region 1 -> region 2

       Unordered geometry:
         area 0 (A0)
	 area 1 (A1 >= A0)
	 area diff (dA = |A0 - A1|)
	 area diff ratio 0 (dA / A0)
	 area diff ratio 1 (dA / A1)
	 approx perim 0 (P0)
	 approx perim 1 (P1)
	 approx perim diff (dP = |P0 - P1|)
	 approx perim diff ratio 0 (dP / P0)
	 approx perim diff ratio 1 (dP / P1)
	 approx boundary length (B)
	 approx boundary perim ratio 0 (B / P0)
	 approx boundary perim ratio 1 (B / P1)
	 compactness 0 (P0 ^ 2 / A0)
	 compactness 1 (P1 ^ 2 / A1)
	 
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
	 Texton:
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
	   
       Saliency:
         sub saliency 0 (S0)
	 sub saliency 1 (S1)
	 sup saliency (S2)
	 saliency abs inc 0 (dS0 = |S2 - S0|)
	 saliency abs inc 1 (dS1 = |S2 - S1|)
	 saliency inc abs diff (|dS0 - dS1|)

     */

    inline void compute (flist& f, double x0, double x1)
    {
      f.push_back(x0);
      f.push_back(x1);
      float dx = fabs(x1 - x0);
      f.push_back(dx);
      f.push_back(getr(dx, x0));
      f.push_back(getr(dx, x1));
    }


    void getUnorderedGeometryFeatures (flist& feat, Points const* r0, 
				       Points const* r1, 
				       Points const* c0, 
				       Points const* c1, 
				       Points const* uc, bool swap01);

    void getIntensityFeatures (flist& feat, Points const& p, 
			       FloatImage::Pointer valImage, 
			       float histLower, float histUpper, 
			       int histBin);

    void getIntensityFeatures (flist& feat, Points const& p0, 
			       Points const& p1, 
			       FloatImage::Pointer valImage, 
			       float histLower, float histUpper, 
			       int histBin);

    void getTextonFeatures (flist& feat, Points const& p0, 
			    Points const& p1, 
			    FloatImage::Pointer valImage, 
			    TextonDict const& tdict, 
			    LabelImage::Pointer canvas);

    // Faster texton feature generation
    void getTextonFeatures 
      (flist& feat, Points const& p0, Points const& p1, 
       TextonDict const& tdict, 
       std::vector<std::vector<TextonPatch> > const& textons, 
       UInt8Image::Pointer check, LabelImage::Pointer canvas);

    void getSaliencyFeatures (flist& feat, float saliency0, 
			      float saliency1, float saliency2, 
			      bool swap01);

  };

};

#endif
