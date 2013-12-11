#ifndef _n3_link3d_bc_h_
#define _n3_link3d_bc_h_

#include "link3d.h"
#include "sc.h"
#include "bs.h"

namespace n3 {

  namespace link3d {

    /* Body classifier features:
       Consider body 0 <-> body 1 (body 0 is below body 1)

       Simple geometry: 

       Advanced geometry:
         Region centroid line angle 0 (with -z axis)
	 Region centroid line angle 1 (with +z axis)
	 Region centroid line included angle
	 Ellipse centroid line angle 0 (with -z axis)
	 Ellipse centroid line angle 1 (with +z axis)
	 Ellipse centroid line included angle 
    */

    /* Canvas should be 1 pixel bigger than real image on each side */
    /* However, imageWidth and imageHeight should be actual image size */
    void getAdvancedGeometry (flist& feat, int id0, int id1, 
			      std::map<int, PointMap> const& rmaps, 
			      BodyStat& bs0, BodyStat& bs1, 
			      fBody const& b0, fBody const& b1, 
			      float dz, cv::Mat& canvas, 
			      int imageWidth, int imageHeight);

  };

};

#endif
