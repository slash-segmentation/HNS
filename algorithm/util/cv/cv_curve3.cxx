#include "cv_curve3.h"
using namespace n3;

// Fit a 3D line with given 3D points
// line = (vx, vy, vz, x0, y0, z0)
// (vx, vy, vz): directional vector; (x0, y0, z0): a point on line
// If checkDirection is true check if line direction needs reverting
// By checking if included angle between fitted line and 
// Line between starting point and mean point is greater than PI / 2 
void n3::fitLine (fvec& line, Points3 const& points, bool checkDirection)
{
  if (points.size() == 0) perr("Error: no point to fit line...");
  line.resize(6, 0.0);
  if (points.size() >= 2) {
    std::vector<cv::Point3f> p;
    p.reserve(points.size());
    for (Points3::const_iterator it = points.begin(); it != points.end(); 
	 ++it) p.push_back(cv::Point3f(it->x, it->y, it->z));
    cv::Vec6f l;
    cv::fitLine(p, l, CV_DIST_L2, 0, 0.01, 0.01);
    line[0] = l[0];
    line[1] = l[1];
    line[2] = l[2];
    line[3] = l[3];
    line[4] = l[4];
    line[5] = l[5];
    if (checkDirection) {
      Point3 cp(points.get_centroid() - points.front());
      if (cp * line < 0) {
	line[0] = -line[0];
	line[1] = -line[1];
	line[2] = -line[2];
      }
    }
  }
  else {
    line[3] = points.front().x;
    line[4] = points.front().y;
    line[5] = points.front().z;
  }
}



// Return included angle of two vectors (vx0, vy0, vz0) and (vx1, vy1, vz1)
// Included angle is within [0, PI]
double n3::getIncludedAngle (double vx0, double vy0, double vz0, 
			     double vx1, double vy1, double vz1)
{
  double den = sqrt((vx0 * vx0 + vy0 * vy0 + vz0 * vz0) * 
		    (vx0 * vx0 + vy0 * vy0 + vz0 * vz0));
  if (den < EPSILON) return DUMMY;
  double num = vx0 * vx1 + vy0 * vy1 + vz0 * vz1;
  double r = num / den;
  if (r > 1.0) r = 1.0;
  else if (r < -1.0) r = -1.0;
  return acos(r);
}
