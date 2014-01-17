#ifndef _n3_cv_curve3_h_
#define _n3_cv_curve3_h_

#include "cv_curve.h"
#include "struct/point3.h"

namespace n3 {

  // Fit a 3D line with given 3D points
  // line = (vx, vy, vz, x0, y0, z0)
  // (vx, vy, vz): directional vector; (x0, y0, z0): a point on line
  // If checkDirection is true check if line direction needs reverting
  // By checking if included angle between fitted line and 
  // Line between starting point and mean point is greater than PI / 2 
  void fitLine (fvec& line, Points3 const& points, bool checkDirection);

  // Return included angle of two vectors (vx0, vy0, vz0) and (vx1, vy1, vz1)
  // Included angle is within [0, PI]
  double getIncludedAngle (double vx0, double vy0, double vz0, 
			   double vx1, double vy1, double vz1);

  // Return included angle of two lines (vx0, vy0, vz0) and (vx1, vy1, vz1)
  // Included angle is within [0, PI]
  double getIncludedAngle (fvec const& l0, fvec const& l1);

};

#endif
