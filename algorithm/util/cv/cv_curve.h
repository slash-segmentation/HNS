#ifndef _n3_cv_curve_h_
#define _n3_cv_curve_h_

#include "n3_util.h"
#include "util/image_util.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace n3 {

  // Input and output points are both in order
  // To average a current point coordinate
  // Use from previous int(3 * sigma) to next int(3 * sigma) points
  void smoothCurve (Points& po, Points const& pi, double sigma = 1.0, 
		    bool closed = false);

  // Smooth each curve individually
  void smoothCurve (std::vector<Points>& po, std::vector<Points> const& pi, 
		    double sigma, bool closed);

  // Approximate curve into corners within specified max deviation distance 
  void approxCurve (Points& corners, Points const& curve, 
		    double maxDeviation, bool closed);

  // Fit a 2D half-line starting at points.front() with other given points
  // line = {vx, vy, points.front().x, points.front().y}
  void fitHalfLine (fvec& line, Points const& points);

  // Fit a 2D line with given 2D points
  // line = {vx, vy, x0, y0}
  // (vx, vy): directional vector; (x0, y0): a point on line
  // If checkDirection is true check if line direction needs reverting
  // By checking if included angle between fitted line and 
  // Line between starting point and mean point is greater than PI / 2 
  void fitLine (fvec& line, Points const& points, bool checkDirection);

  // Return included angle of two vectors (vx0, vy0) and (vx1, vy1)
  // Included angle is within [0, PI]
  // Return DUMMY if encounter dividing zero problem
  double getIncludedAngle (double vx0, double vy0, double vx1, double vy1);

  // *** Assume boundary and contour have same direction
  // Fit half lines at junction point
  // bline: boundary line
  // cline: contour line
  // isJpBoundaryHead: junction is boundary head (true) or tail (false) 
  // cjp: oc's iterator of junction point
  void getJunctionLines (fvec& bline, fvec& cline, Points const& oc, 
			 Points const& ob, bool isJpBoundaryHead, 
			 Points::const_iterator cjp, int fitPointNum, 
			 bool smooth);

  // Compute included angles at junction points of two contours
  // Ignore junctions on image borders
  // Each angle vector = {a(c0, c1), a(c0, b0), a(c1, b1), a(b0, b1)}
  // Only consider matched junction points
  void getJunctions
    (std::vector<Point>& jpoints, std::vector<fvec>& jangles, 
     Points const& oc0, Points const& oc1, 
     std::vector<Points> const& ob0, std::vector<Points> const& ob1, 
     std::vector<Points::const_iterator> const& head0, 
     std::vector<Points::const_iterator> const& head1, 
     std::vector<Points::const_iterator> const& tail0, 
     std::vector<Points::const_iterator> const& tail1, 
     int fitPointNum, int imageWidth, int imageHeight, bool smooth);

  // Compute signed rotational angle (within [-PI, PI], CCW being positive)
  double getRotAngle (double vfromx, double vfromy, 
		      double vtox, double vtoy);

  // Fit an ellipse with (center, width, height, rotAngle)
  void fitEllipse (RotBox& ebox, Points const& contour);

  // cw should be true if vertices are:
  // Clockwise if origin is bottom-left and x to right and y upwards
  // Counterclockwise, if origin is top-left and x to right and y downwards
  // For vertices generated using OpenCV findContours, cw = true
  // Internal angles are always positive
  template <typename TContainer> void 
    getInternalAngles (TContainer& angles, Points const& vertices, 
		       bool cw = true)
    {
      if (vertices.size() < 3) return;
      Points::const_iterator vit0 = vertices.last();
      Points::const_iterator vit1 = vertices.begin();
      Points::const_iterator vit2 = vertices.next(vit1, true);
      bool first = true;
      while (vit1 != vertices.begin() || first) {
	first = false;
	Point vfrom = *vit1 - *vit0, vto = *vit2 - *vit1;
	double angle = getRotAngle(vfrom.x, vfrom.y, vto.x, vto.y);
	// Handle concave internal angles: must bigger than PI
	if (cw) angle += PI;
	else angle = PI - angle;
	angles.insert(angles.end(), angle);
	vit0 = vit1;
	vit1 = vit2;
	vit2 = vertices.next(vit2, true);
      }
    }

};

#endif
