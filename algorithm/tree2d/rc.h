#ifndef _n3_tree2d_rc_h_
#define _n3_tree2d_rc_h_

#include "bc.h"

namespace n3 {

  namespace tree2d {

    const unsigned int RC_RI_HIST_BIN = BC_RI_HIST_BIN;
    const unsigned int RC_IA_HIST_BIN = BC_CA_HIST_BIN * 2;
    
    /* Region classifier features: 
       Consider: one region

       Shape: [2 - 4, 5 - 11, 12 - 26, 27 - 32, 33 - 37]
         whether touches image border
         area
	 perimeter
	 compactness
	 Hu moments
	 convexity defect min
	 convexity defect max
	 convexity defect mean
	 convexity defect median
	 convexity defect std dev
	 fitted ellipse box area
	 fitted ellipse box width
	 fitted ellipse box height
	 fitted ellipse box width / height
	 min area box area
	 min area box width
	 min area box height
	 min area box width / height
	 min enclosing circle radius
	 contour number
	 contour internal angle histogram
	 contour internal angle min
	 contour internal angle max
	 contour internal angle mean
	 contour internal angle median 
	 contour internal angle std dev

       Raw intensity: [38 - 47, 48 - 52]
         contour raw intensity histogram
	 contour raw intensity min
	 contour raw intensity max
	 contour raw intensity mean
	 contour raw intensity median
	 contour raw intensity std dev
     */

    void genRegionFeature (flist& feat, Points const& region, 
			   Points& contour, FloatImage::Pointer rawImage, 
			   cv::Mat& canvas, bool useIntensity);

  };

};

#endif
