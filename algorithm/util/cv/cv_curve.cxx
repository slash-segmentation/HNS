#include "cv_curve.h"
using namespace n3;

// Input and output points are both in order
// To average a current point coordinate
// Use from previous int(3 * sigma) to next int(3 * sigma) points
void n3::smoothCurve (Points& po, Points const& pi, double sigma, 
		      bool closed)
{  
  int n = pi.size();
  int radius = (int)(sigma * 3.0);
  if (n < 2 * radius + 1) po = pi; // Do not smooth very short curves
  else {
    const double sqrt2pi_sigma = sqrt(PI * 2.0) * sigma; 
    const double sigma22 = sigma * sigma * 2.0;
    int i = 0;
    Points::const_iterator iit = pi.begin();
    while (iit != pi.end()) {
      int dpre_max = (!closed && i - radius < 0? i: radius);
      int dpost_max = (!closed && i + radius >= n? n - i - 1: radius);
      int dpre = std::min(dpre_max, dpost_max);
      int dpost = dpre;
      int len = dpost + dpre + 1;
      double x[len], y[len], w[len], wsum = 0.0;
      Points::const_iterator endpos = pi.end(); --endpos; // Last point
      // Move to contributing point
      for (int d = -dpre; d <= dpost; ++d) {
	Points::const_iterator cit = iit;
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
	double dist = iit->get_distance(*cit);
	w[k] = exp(-dist * dist / sigma22) / sqrt2pi_sigma;
	wsum += w[k];
      }
      // Weight sum and normalize
      double ox = 0.0, oy = 0.0;
      for (int j = 0; j < len; ++j) {
	ox += (x[j] * w[j]);
	oy += (y[j] * w[j]);
      }
      ox /= wsum;
      oy /= wsum;
      po.push_back(Point((float)ox, (float)oy));
      ++i;
      ++iit;
    }
    // po.unique(); // Remove duplicate points
  }
}



// Smooth each curve individually
void n3::smoothCurve (std::vector<Points>& po, 
		      std::vector<Points> const& pi, 
		      double sigma, bool closed)
{
  po.reserve(pi.size());
  for (std::vector<Points>::const_iterator it = pi.begin(); 
       it != pi.end(); ++it) {
    Points p;
    smoothCurve(p, *it, sigma, closed);
    po.push_back(Points());
    splice(po.back(), p, false);
  }
}



// Smooth out sharp angles smaller than threshold ([0, PI])
void n3::smoothSharpCurve (Points& po, double threshold, bool closed)
{
  if (po.size() < 3) return;
  Points::iterator iit0, iit1, iit2;
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




// Approximate curve into corners within specified max deviation distance 
void n3::approxCurve (Points& corners, Points const& curve, 
		      double maxDeviation, bool closed)
{
  std::vector<cv::Point> input, output;
  input.reserve(curve.size());
  for (Points::const_iterator it = curve.begin(); it != curve.end(); ++it)
    input.push_back(cv::Point(it->x, it->y));
  cv::approxPolyDP(input, output, maxDeviation, closed);
  for (std::vector<cv::Point>::const_iterator it = output.begin(); 
       it != output.end(); ++it) 
    corners.push_back(Point(it->x, it->y));
}



// Fit a 2D half-line starting at points.front() with other given points
// line = {vx, vy, points.front().x, points.front().y}
void n3::fitHalfLine (fvec& line, Points const& points)
{
  if (points.size() == 0) perr("Error: no point to fit line...");
  line.resize(4, 0.0);  
  Point p0 = points.front();
  line[2] = p0.x;
  line[3] = p0.y;
  if (points.size() >= 2) {
    double sxd = 0.0, syd = 0.0, sxd2 = 0.0, sxdyd = 0.0;
    Points::const_iterator it = points.next(points.begin(), false);
    while (it != points.end()) {
      Point d = *it - p0;
      sxd += (double)d.x;
      syd += (double)d.y;
      sxd2 += (double)d.x * (double)d.x;
      sxdyd += (double)d.x * (double)d.y;
      ++it;
    }
    line[0] = sxd2;
    line[1] = sxdyd;
    if (fabs(line[0]) < EPSILON && fabs(line[1]) < EPSILON) 
      line[1] = 1.0;
    if (line[0] * sxd + line[1] * syd < 0.0) {
      line[0] = -line[0];
      line[1] = -line[1];
    }
  }
}



// Fit a 2D line with given 2D points
// line = (vx, vy, x0, y0)
// (vx, vy): directional vector; (x0, y0): a point on line
// If checkDirection is true check if line direction needs reverting
// By checking if included angle between fitted line and 
// Line between starting point and mean point is greater than PI / 2 
void n3::fitLine (fvec& line, Points const& points, bool checkDirection)
{
  if (points.size() == 0) perr("Error: no point to fit line...");
  line.resize(4, 0.0);
  if (points.size() >= 2) {
    std::vector<cv::Point2f> p;
    p.reserve(points.size());
    for (Points::const_iterator it = points.begin(); it != points.end(); 
	 ++it) p.push_back(cv::Point2f(it->x, it->y));
    cv::Vec4f l;
    cv::fitLine(p, l, CV_DIST_L2, 0, 0.01, 0.01);  
    line[0] = l[0];
    line[1] = l[1];
    line[2] = l[2];
    line[3] = l[3];
    if (checkDirection) {
      Point cp(points.get_centroid() - points.front());
      if (cp * line < 0) {
	line[0] = -line[0];
	line[1] = -line[1];
      }
    }
  }
  else {
    line[2] = points.front().x;
    line[3] = points.front().y;
  }
}



// Return included angle of two vectors (vx0, vy0) and (vx1, vy1)
// Included angle is within [0, PI]
// Return DUMMY if encounter dividing zero problem
double n3::getIncludedAngle (double vx0, double vy0, 
			     double vx1, double vy1)
{
  double den = sqrt((vx0 * vx0 + vy0 * vy0) * (vx1 * vx1 + vy1 * vy1));
  if (den < EPSILON) return DUMMY;
  double num = vx0 * vx1 + vy0 * vy1;
  double r = num / den;
  // Prevent numerical problem: r >~ 1 or r ~< -1
  if (r > 1.0) r = 1.0;
  else if (r < -1.0) r = -1.0;
  return acos(r);
}



// *** Assume boundary and contour have same direction
// Fit lines at junction point
// bline: boundary line
// cline: contour line
// isJpBoundaryHead: junction is boundary head (true) or tail (false) 
// cjp: oc's iterator of junction point
void n3::getJunctionLines (fvec& bline, fvec& cline, Points const& oc, 
			   Points const& ob, bool isJpBoundaryHead, 
			   Points::const_iterator cjp, int fitPointNum, 
			   bool smooth)
{
  // Fit bline
  Points bp;
  if (isJpBoundaryHead) {
    int cnt = 0;
    for (Points::const_iterator it = ob.begin(); it != ob.end(); ++it) {
      bp.push_back(*it);
      if (++cnt >= fitPointNum) break; 
    }
  }
  else {
    int cnt = 0;
    for (Points::const_reverse_iterator it = ob.rbegin(); 
	 it != ob.rend(); ++it) {
      bp.push_back(*it);
      if (++cnt >= fitPointNum) break;
    }
  }
  if (smooth) {
    Points bps;
    smoothCurve(bps, bp, 1.0, false);
    fitHalfLine(bline, bps);
  }
  else fitHalfLine(bline, bp);
  Points cp;
  if (isJpBoundaryHead) {
    int cnt = 0;
    Points::const_iterator it = cjp;
    bool first = true;
    while (it != cjp || first) {
      first = false;
      cp.push_back(*it);
      if (++cnt >= fitPointNum) break;
      it = oc.previous(it, true);
    }    
  }
  else {
    int cnt = 0;
    Points::const_iterator it = cjp;
    bool first = true;
    while (it != cjp || first) {
      first = false;
      cp.push_back(*it);
      if (++cnt >= fitPointNum) break;
      it = oc.next(it, true);
    }
  }
  if (smooth) {
    Points cps;
    smoothCurve(cps, cp, 1.0, false);
    fitHalfLine(cline, cps);
  }
  else fitHalfLine(cline, cp);
}


// *** Assume boundary and contour have same direction
// Fit half lines at junction point
// Compute included angles at junction points of two contours
// Ignore junctions on image borders
// Each angle vector = {a(c0, c1), a(c0, b0), a(c1, b1), a(b0, b1)}
// Only consider matched junction points
void n3::getJunctions 
(std::vector<Point>& jpoints, std::vector<fvec>& jangles, 
 Points const& oc0, Points const& oc1, 
 std::vector<Points> const& ob0, std::vector<Points> const& ob1, 
 std::vector<Points::const_iterator> const& head0, 
 std::vector<Points::const_iterator> const& head1, 
 std::vector<Points::const_iterator> const& tail0, 
 std::vector<Points::const_iterator> const& tail1, 
 int fitPointNum, int imageWidth, int imageHeight, bool smooth)
{
  if (ob0.front().front().is_neighbor(ob0.back().back()) || 
      ob1.front().front().is_neighbor(ob1.back().back())) return;
  std::vector<bool> nb0h(ob0.size()), nb0t(ob0.size());
  std::vector<bool> nb1h(ob1.size()), nb1t(ob1.size());
  for (int i = 0; i < ob0.size(); ++i) {
    nb0h[i] = !isborder(ob0[i].front(), imageWidth, imageHeight);
    nb0t[i] = !isborder(ob0[i].back(), imageWidth, imageHeight);
  }
  for (int i = 0; i < ob1.size(); ++i) {    
    nb1h[i] = !isborder(ob1[i].front(), imageWidth, imageHeight);
    nb1t[i] = !isborder(ob1[i].back(), imageWidth, imageHeight);
  }
  // Match type: hh - 1, ht - 2, th - 3, tt - 4
  std::list<int> midx0, midx1, mtype;
  for (int i = 0; i < ob0.size(); ++i) 
    for (int j = 0; j < ob1.size(); ++j) {
      if (nb0h[i] && nb1h[j] && 
	  ob0[i].front().is_neighbor(ob1[j].front())) {
	midx0.push_back(i);
	midx1.push_back(j);
	mtype.push_back(1);
      }
      else if (nb0h[i] && nb1t[j] && 
	       ob0[i].front().is_neighbor(ob1[j].back())) {
	midx0.push_back(i);
	midx1.push_back(j);
	mtype.push_back(2);
      }
      if (nb0t[i] && nb1h[j] && 
	  ob0[i].back().is_neighbor(ob1[j].front())) {
	midx0.push_back(i);
	midx1.push_back(j);
	mtype.push_back(3);
      }
      else if (nb0t[i] && nb1t[j] && 
	       ob0[i].back().is_neighbor(ob1[j].back())) {
	midx0.push_back(i);
	midx1.push_back(j);
	mtype.push_back(4);
      }
    }
  // Compute junction information
  int n = mtype.size();
  jpoints.reserve(n);
  jangles.reserve(n);
  std::list<int>::const_iterator mit0 = midx0.begin();
  std::list<int>::const_iterator mit1 = midx1.begin(); 
  std::list<int>::const_iterator tit = mtype.begin(); 
  while (tit != mtype.end()) {
    bool ishead0 = (*tit == 1 || *tit == 2);
    bool ishead1 = (*tit == 1 || *tit == 3);    
    jpoints.push_back(((ishead0? ob0[*mit0].front(): ob0[*mit0].back()) + 
		       (ishead1? ob1[*mit1].front(): ob1[*mit1].back())) 
		      / 2.0);
    fvec bl0, cl0, bl1, cl1;
    getJunctionLines(bl0, cl0, oc0, ob0[*mit0], ishead0, 
		     (ishead0? head0[*mit0]: tail0[*mit0]), 
		     fitPointNum, smooth);
    getJunctionLines(bl1, cl1, oc1, ob1[*mit1], ishead1, 
		     (ishead1? head1[*mit1]: tail1[*mit1]), 
		     fitPointNum, smooth);
    fvec angles(4);
    angles[0] = getIncludedAngle(cl0[0], cl0[1], cl1[0], cl1[1]);
    angles[1] = getIncludedAngle(cl0[0], cl0[1], bl0[0], bl0[1]);
    angles[2] = getIncludedAngle(cl1[0], cl1[1], bl1[0], bl1[1]);
    angles[3] = getIncludedAngle(bl0[0], bl0[1], bl1[0], bl1[1]);
    jangles.push_back(angles);
    ++mit0;
    ++mit1;
    ++tit;
  }
  
}



// Compute signed rotational angle (within [-PI, PI], CCW being positive)
double n3::getRotAngle (double vfromx, double vfromy, 
			double vtox, double vtoy)
{
  if (fabs(vfromx) < EPSILON && fabs(vfromy) < EPSILON && 
      fabs(vtox) < EPSILON && fabs(vtoy) < EPSILON) return 0.0;
  double afrom = atan2(vfromy, vfromx);
  double ato = atan2(vtoy, vtox);
  double ret = ato - afrom;
  if (ret > PI) ret -= 2.0 * PI;
  else if (ret < -PI) ret += 2.0 * PI;
  return ret;
}



// Fit an ellipse with (center, width, height, rotAngle)
void n3::fitEllipse (RotBox& ebox, Points const& contour)
{
  if (contour.size() >= 5) {
    std::vector<cv::Point> c;
    c.reserve(contour.size());
    for (Points::const_iterator it = contour.begin(); 
	 it != contour.end(); ++it)
      c.push_back(cv::Point(it->x, it->y));
    cv::RotatedRect el = cv::fitEllipse(c);
    ebox.center.x = el.center.x;
    ebox.center.y = el.center.y;
    ebox.width = el.size.width;
    ebox.height = el.size.height;
    ebox.angle = el.angle / PI;
  }
  else {
    ebox.start.x = -1.0;
    ebox.start.y = -1.0;
    ebox.center.x = -1.0;
    ebox.center.y = -1.0;
    ebox.width = 0.0;
    ebox.height = 0.0;
    ebox.angle = 0.0;
  }
}
