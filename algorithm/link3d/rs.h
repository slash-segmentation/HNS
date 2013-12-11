#ifndef _n3_link3d_rs_h_
#define _n3_link3d_rs_h_

#include "util/cv/cv_contour.h"
#include "util/cv/cv_curve.h"
#include "util/stat_util.h"

namespace n3 {

  namespace link3d {

    const unsigned int RS_IA_HIST_BIN = 12;
    const unsigned int RS_IN_HIST_BIN = 10;

    struct RegionStat {
      bool border; /* If region touches image borders or not */
      Points points; /* Region points */
      Points ucontour; /* Unordered contour */
      std::vector<Points> ocontour; /* Ordered contour */
      std::vector<Points> polygon;
      Point centroid;
      Box box;
      std::vector<fvec> stats;
      /* Region statistics in use:
	 [0] area (A)
	 [1] perimeter (P)
	 [2] compactness (P ^ 2 / A)
	 [3] Hu moments
	 [4] convexity defect depths
	 [5] convexity defect depths min/max/mean/median/std dev
	 [6] fitted ellipse box width/height
	 [7] min area box width/height
	 [8] min elosing circle radius
	 [9] contour internal angles
	 [10] contour internal angle histogram
	 [11] contour internal angle min/max/mean/median/std dev
	 Region statistics in use for tree3d only:
	 [12] contour raw intensity histogram
	 [13] contour raw intensity min/max/mean/median/std dev
	 [14] contour pb intensity histogram
	 [15] contour pb intensity min/max/mean/median/std dev
	 [16] contour texton histogram
      */

      friend std::ostream& operator << 
      (std::ostream& os, RegionStat const& rs) {
	os << "border = " << (rs.border? "true": "false") << "\n";
	os << "points (" << rs.points.size() << ")\n";
	os << "ucontour (" << rs.ucontour.size() << ") = {" 
	   << rs.ucontour << "}\n"; 
	os << "ocontour (" << rs.ocontour.size() << "):\n";
	for (int i = 0; i < rs.ocontour.size(); ++i) 
	  os << i << ": {" << rs.ocontour[i] << "}\n";
	os << "polygon (" << rs.polygon.size() << "):\n";
	for (int i = 0; i < rs.polygon.size(); ++i) 
	  os << i << ": {" << rs.polygon[i] << "}\n";
	os << "centroid = " << rs.centroid << "\n"; 
	os << "box = " << rs.box << "\n";
	os << "stats:\n";
	for (int i = 0; i < rs.stats.size(); ++i) {
	  os << "[" << i << "]: ";
	  for (fvec::const_iterator it = rs.stats[i].begin(); it != 
		 rs.stats[i].end(); ++it) os << *it << " ";
	  os << "\n";
	}
	return os;
      }
    };

    /* Gather region points */
    void getRegionPoints (std::map<Label, RegionStat>& smap, 
			  LabelImage::Pointer labelImage);

    // Region and unordered contour points should have been initialized
    void getRegionStat (RegionStat& rs, cv::Mat& canvas, 
			int imageWidth, int imageHeight);

  };

};

#endif
