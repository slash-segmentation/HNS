#ifndef _n3_link3d_sc_h_
#define _n3_link3d_sc_h_

#include "util/struct_util.h"
#include "util/cv/cv_contour.h"
#include "link3d.h"
#include "rs.h"

namespace n3 {

  namespace link3d {

    /* Section classifier features:  
       Consider region 0 <-> region 1
       
       Simple geometry (29 features):
	 area0 (A0)
	 area1 (A1 >= A0)
	 area abs diff (dA = |A0 - A1|)
	 area ratio (A0 / A1)
	 area abs diff ratio0 (dA / A0)
	 area abs diff ratio1 (dA / A1)
	 overlap (O)
	 overlap ratio 0 (O / A0)
	 overlap ratio 1 (O / A1)
	 sym diff (S)
	 sym diff union area ratio (S / (A0 + A1 - O))
	 sym diff ratio0 (S / A0)
	 sym diff ratio1 (S / A1)
	 approx perim 0 (AP0)
	 approx perim 1 (AP1)
	 approx perim abs diff (dAP = |AP0 - AP1|)
	 approx perim ratio (AP0 / AP1)
	 approx perim abs diff ratio 0 (dAP / AP0)
	 approx perim abs diff ratio 1 (dAP / AP1)
	 centroid dist (Cd)
	 centroid dist approx perim ratio 0 (Cd / AP0)
	 centroid dist approx perim ratio 1 (Cd / AP1)
	 centroid dist approx perim sum ratio (Cd / (AP0 + AP1))
	 centroid dist area ratio 0 (Cd / sqrt(A0))
	 centroid dist area ratio 1 (Cd / sqrt(A1))
	 centroid dist area sum ratio (Cd / (sqrt(A0) + sqrt(A1)))
	 approx compactness (ACp0 = AP0 ^ 2 / A0)
	 approx compactness (ACp1 = AP1 ^ 2 / A1)
	 approx compactness abs diff (|ACp0 - ACp1|)
	 
       Advanced geometry (172 features):
         is region 0 on border
	 is region 1 on border
	 perim 0 (P0)
	 perim 1 (P1)
	 perim abs diff (dP = |P0 - P1|)
	 perim ratio (P0 / P1)
	 perim abs diff ratio 0 (dP / P0)
	 perim abs diff ratio 1 (dP / P1)
	 centroid dist perim ratio 0 (Cd / P0)
	 centroid dist perim ratio 1 (Cd / P1)
	 centroid dist perim sum ratio (Cd / (P0 + P1))
	 compactness 0 (Cp0)
	 compactness 1 (Cp1)
	 compactness abs diff (|Cp0 - Cp1|)
	 Hu moments 0
	 Hu moments 1
	 Hu moments abs diff
	 Hu moments based shape matching confidence (method 1)
	 Hu moments based shape matching confidence (method 2)
	 Hu moments based shape matching confidence (method 3)
	 convexity defect min 0
	 convexity defect min 1
	 convexity defect min abs diff
	 convexity defect max 0
	 convexity defect max 1
	 convexity defect max abs diff
	 convexity defect mean 0
	 convexity defect mean 1
	 convexity defect mean abs diff
	 convexity defect median 0
	 convexity defect median 1
	 convexity defect median abs diff
	 convexity defect std dev 0
	 convexity defect std dev 1
	 convexity defect std dev abs diff
	 bounding box width 0
	 bounding box width 1
	 bounding box width abs diff
	 bounding box width ratio
	 bounding box height 0
	 bounding box height 1
	 bounding box height abs diff
	 bounding box height ratio
	 bounding box width/height ratio 0
	 bounding box width/height ratio 1
	 bounding box width/height ratio abs diff
	 bounding box area 0
	 bounding box area 1
	 bounding box area abs diff
	 bounding box area ratio
	 bounding box fill ratio 0
	 bounding box fill ratio 1
	 bounding box fill ratio abs diff
	 fitted ellipse box width 0
	 fitted ellipse box width 1
	 fitted ellipse box width abs diff
	 fitted ellipse box width ratio
	 fitted ellipse box height 0
	 fitted ellipse box height 1
	 fitted ellipse box height abs diff
	 fitted ellipse box height ratio
	 fitted ellipse box width/height ratio 0
	 fitted ellipse box width/height ratio 1
	 fitted ellipse box width/height ratio abs diff
	 fitted ellipse box area 0
	 fitted ellipse box area 1
	 fitted ellipse box area abs diff
	 fitted ellipse box area ratio 
	 fitted ellipse box fill ratio 0
	 fitted ellipse box fill ratio 1
	 fitted ellipse box fill ratio abs diff
	 min area box width 0
	 min area box width 1
	 min area box width abs diff
	 min area box width ratio
	 min area box height 0
	 min area box height 1
	 min area box height abs diff
	 min area box height ratio
	 min area box width/height ratio 0
	 min area box width/height ratio 1
	 min area box width/height ratio abs diff
	 min area box area 0
	 min area box area 1
	 min area box area abs diff
	 min area box area ratio
	 min area box fill ratio 0
	 min area box fill ratio 1
	 min area box fill ratio abs diff	 
	 min enclosing circle radius 0
	 min enclosing circle radius 1
	 min enclosing circle radius abs diff 
	 min enclosing circle radius ratio
	 min enclosing circle area 0
	 min enclosing circle area 1
	 min enclosing circle area abs diff
	 min enclosing circle area ratio
	 min enclosing circle fill ratio 0
	 min enclosing circle fill ratio 1
	 min enclosing circle fill ratio abs diff
	 contour number 0
	 contour number 1
	 contour number abs diff
	 contour internal angle hist 0
	 contour internal angle hist 1	 
	 contour internal angle hist abs diff
	 contour internal angle min 0
	 contour internal angle min 1
	 contour internal angle min abs diff
	 contour internal angle max 0
	 contour internal angle max 1
	 contour internal angle max abs diff
	 contour internal angle mean 0
	 contour internal angle mean 1
	 contour internal angle mean abs diff
	 contour internal angle median 0
	 contour internal angle median 1
	 contour internal angle median abs diff
	 contour internal angle std dev 0
	 contour internal angle std dev 1
	 contour internal angle std dev abs diff

       Intensity (47 features):
         hist 0
	 hist 1
	 hist abs diff
	 hist L1 dist
	 hist X2 dist
	 min 0
	 min 1
	 min abs diff
	 max 0
	 max 1
	 max abs diff
	 mean 0
	 mean 1
	 mean abs diff
	 median 0
	 median 1
	 median abs diff
	 std dev 0
	 std dev 1
	 std dev abs diff

       Texture (2 features):
         texton hist L1 dist
	 texton hist X2 dist

       IN USE:
         Simple geometry for non-aligned region pair [1 - 29]
	 Advanced geometry [30 - 201]
	 Simple geometry for aligned region pair [202 - 230]       
	 
     */



    // Add x0, x1, dx, x0 / x1, dx / x0, dx / x1
    inline void compute (flist& feat, double x0, double x1)
    {
      feat.push_back(x0);
      feat.push_back(x1);
      double dx = fabs(x0 - x1);
      feat.push_back(dx);
      feat.push_back(getr(x0, x1));
      feat.push_back(getr(dx, x0));
      feat.push_back(getr(dx, x1));
    }



    // For each stat, add x0, x1, |x0 - x1|
    inline void compute (flist& feat, fvec const& stats0, 
			 fvec const& stats1)
    {
      int n = stats0.size();
      for (int i = 0; i < n; ++i) {
	feat.push_back(stats0[i]);
	feat.push_back(stats1[i]);
	feat.push_back(fabs(stats0[i] - stats1[i]));
      }
    }



    inline void compute (flist& feat, double w0, double h0, double w1, 
			 double h1, double a0, double a1)
    {
      feat.push_back(w0);
      feat.push_back(w1);
      feat.push_back(w0 != DUMMY && w1 != DUMMY? fabs(w0 - w1): DUMMY);
      feat.push_back(w0 != DUMMY && w1 != DUMMY? getr(w0, w1): DUMMY);
      feat.push_back(h0);
      feat.push_back(h1);
      feat.push_back(h0 != DUMMY && h1 != DUMMY? fabs(h0 - h1): DUMMY);
      feat.push_back(h0 != DUMMY && h1 != DUMMY? getr(h0, h1): DUMMY);
      double r0 = w0 != DUMMY && h0 != DUMMY? getr(w0, h0): DUMMY;
      double r1 = w1 != DUMMY && h1 != DUMMY? getr(w1, h1): DUMMY;
      feat.push_back(r0);
      feat.push_back(r1);
      feat.push_back(r0 != DUMMY && r1 != DUMMY? fabs(r0 - r1): DUMMY);
      double b0 = w0 != DUMMY && h0 != DUMMY? w0 * h0: DUMMY;
      double b1 = w1 != DUMMY && h1 != DUMMY? w1 * h1: DUMMY;
      feat.push_back(b0);
      feat.push_back(b1);
      feat.push_back(b0 != DUMMY && b1 != DUMMY? fabs(b0 - b1): DUMMY);
      feat.push_back(b0 != DUMMY && b1 != DUMMY? getr(b0, b1): DUMMY);
      double f0 = b0 != DUMMY? getr(a0, b0): DUMMY;
      double f1 = b1 != DUMMY? getr(a1, b1): DUMMY;
      feat.push_back(f0);
      feat.push_back(f1);
      feat.push_back(f0 != DUMMY && f1 != DUMMY? fabs(f0 - f1): DUMMY);
    }



    void getSimpleGeometry (flist& feat, double a0, double a1, 
			    double ap0, double ap1, double cd, 
			    double overlap, bool swap01);

    void getAdvancedGeometry (flist& feat, RegionStat const* rs0, 
			      RegionStat const* rs1, bool swap01);

  };

};


#endif

