#include "cv_curve3.h"
using namespace n3;

// Input and output points are both in order
// To average a current point coordinate
// Use from previous int(3 * sigma) to next int(3 * sigma) points
void n3::smoothCurve (Points3& po, Points3 const& pi, double sigma, 
		      bool closed)
{  
  int n = pi.size();
  int radius = (int)(sigma * 3.0);
  if (n < 2 * radius + 1) po = pi; // Do not smooth very short curves
  else {
    const double sqrt2pi_sigma = sqrt(PI * 2.0) * sigma; 
    const double sigma22 = sigma * sigma * 2.0;
    int i = 0;
    Points3::const_iterator iit = pi.begin();
    while (iit != pi.end()) {
      int dpre_max = (!closed && i - radius < 0? i: radius);
      int dpost_max = (!closed && i + radius >= n? n - i - 1: radius);
      int dpre = std::min(dpre_max, dpost_max);
      int dpost = dpre;
      int len = dpost + dpre + 1;
      double x[len], y[len], z[len], w[len], wsum = 0.0;
      Points3::const_iterator endpos = pi.end(); --endpos; // Last point
      // Move to contributing point
      for (int d = -dpre; d <= dpost; ++d) {
	Points3::const_iterator cit = iit;
	if (d < 0) 
	  for (int j = 0; j < -d; j++) 
	    if (cit == pi.begin()) cit = endpos;
	    else --cit;
	else 
	  for (int j = 0; j < d; j++) 
	    if (cit == endpos) cit = pi.begin(); 
	    else ++cit;	
	// Collect coordinate and weight
	int k = d + dpre;
	x[k] = (double)cit->x;
	y[k] = (double)cit->y;
	z[k] = (double)cit->z;
	double dist = iit->get_distance(*cit);
	w[k] = exp(-dist * dist / sigma22) / sqrt2pi_sigma;
	wsum += w[k];
      }
      // Weight sum and normalize
      double ox = 0.0, oy = 0.0, oz = 0.0;
      for (int j = 0; j < len; ++j) {
	ox += (x[j] * w[j]);
	oy += (y[j] * w[j]);
	oz += (z[j] * w[j]);
      }
      ox /= wsum;
      oy /= wsum;
      oz /= wsum;
      po.push_back(Point3((float)ox, (float)oy, (float)oz));
      ++i;
      ++iit;
    }
    // po.unique(); // Remove duplicate points
  }
}


// Smooth out sharp angles smaller than threshold ([0, PI])
void n3::smoothSharpCurve (Points3& po, double threshold, bool closed)
{
  if (po.size() < 3) return;
  Points3::iterator iit0, iit1, iit2;
  if (closed) {
    iit0 = po.end(); --iit0;
    iit1 = po.begin(); 
    iit2 = iit1; ++iit2;
  }
  else {
    iit0 = po.begin();
    iit1 = iit0; ++iit1;
    iit2 = iit1; ++iit2;
  }
  bool repeat = true;
  while (repeat) {
    repeat = false;
    flist angles;
    po.get_curve_angles(angles, closed);
    flist::const_iterator ait = angles.begin();
    while (ait != angles.end()) {
      if (*ait < threshold) {
	repeat = true;
	*iit1 = ((*iit0 + *iit2) / 2.0 + *iit1) / 2.0;
      }
      ++ait;
      iit0 = iit1;
      iit1 = iit2;
      ++iit2;
      if (closed && iit2 == po.end()) iit2 = po.begin();
    }
  }
}



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



// Return included angle of two lines (vx0, vy0, vz0) and (vx1, vy1, vz1)
// Included angle is within [0, PI]
double n3::getIncludedAngle (fvec const& l0, fvec const& l1)
{
  return getIncludedAngle(l0[0], l0[1], l0[2], l1[0], l1[1], l1[2]);
}
