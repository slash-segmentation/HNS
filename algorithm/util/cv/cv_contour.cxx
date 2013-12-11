#include "cv_contour.h"
using namespace n3;

/* Connect should be 4 or 8 */
/* If returned vector is not full, point must be on image border */
std::vector<Pixel<uchar> > 
n3::getNeighbors (Point const& p, cv::Mat const& im, int connect)
{
  int w = im.cols, h = im.rows;
  int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
  int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
  typedef Pixel<uchar> TPixel;
  std::vector<TPixel> ret;
  int inc = (connect == 8? 1: 2);
  ret.reserve(connect == 8? 8: 4);
  for (int i = 0; i < 8; i += inc) {
    int x = p.x + dx[i], y = p.y + dy[i];
    if (x >= 0 && x < w && y >= 0 && y < h) 
      ret.push_back(TPixel(x, y, im.at<uchar>(y, x)));
  }
  return ret;
}



// Get ordered contours of a region
// Only consider contour points (cp) that are 4-adjacent to region (rp)
// First contour is outmost
// Set xoff/yoff >= 1 to include image border points 
// Optionally paint rp/cp by giving non-NULL pointers
// Only when resetCanvas == true canvas is set to 0
void n3::getContour (std::vector<Points>& contour, cv::Mat& canvas, 
		     Points const* rp, Points const* cp, 
		     int xoff, int yoff, bool resetCanvas)
{
  if (resetCanvas) canvas.setTo(cv::Scalar(0));
  if (rp != NULL) 
    for (Points::const_iterator it = rp->begin(); it != rp->end(); ++it) 
      canvas.at<uchar>(it->y + yoff, it->x + xoff) = 1;
  if (cp != NULL) 
    for (Points::const_iterator it = cp->begin(); it != cp->end(); ++it)
      if (adj4(*it, xoff, yoff, 1, canvas)) 
	canvas.at<uchar>(it->y + yoff, it->x + xoff) = 2;
  cv::vector<cv::vector<cv::Point> > _c;
  cv::vector<cv::Vec4i> _h;
  cv::findContours(canvas, _c, _h, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, 
		   cvPoint(-xoff, -yoff));
  int cnum = _c.size();
  contour.reserve(cnum);
  int outer = -1;
  for (int i = 0; i < cnum; i++)
    if (_h[i][3] < 0) {
      outer = i;
      break;
    }
  contour.push_back(Points());
  for (int j = 0; j < _c[outer].size(); ++j)
    contour.back().push_back(Point(_c[outer][j].x, _c[outer][j].y));
  for (int i = 0; i < cnum; ++i)
    if (i != outer) {
      contour.push_back(Points());
      for (int j = 0; j < _c[i].size(); ++j) 
	contour.back().push_back(Point(_c[i][j].x, _c[i][j].y));
    }
}



// Given region points find unordered outer contour points
// Do not include image border points
// Actual image size is controlled by imageWidth/imageHeight
// If canvas has been painted set rp = NULL
// Connect should be equal to CRCONN for consistency
void n3::getContour (Points& contour, cv::Mat& canvas, Points const* rp, 
		     int connect, int imageWidth, int imageHeight, 
		     int xoff, int yoff, bool resetCanvas)
{
  if (resetCanvas) canvas.setTo(cv::Scalar(0));
  if (rp != NULL)     
    for (Points::const_iterator it = rp->begin(); it != rp->end(); 
	 ++it) canvas.at<uchar>(it->y + yoff, it->x + xoff) = 1;
  for (Points::const_iterator rit = rp->begin(); 
       rit != rp->end(); ++rit) {
    Point p(rit->x + xoff, rit->y + yoff);
    std::vector<Pixel<uchar> > n = getNeighbors(p, canvas, connect);
    for (std::vector<Pixel<uchar> >::const_iterator nit = n.begin(); 
	 nit != n.end(); ++nit) 
      if (nit->val == 0) {
	float x = nit->x - xoff, y = nit->y - yoff;
	if (x >= 0 && x < imageWidth && y >= 0 && y < imageHeight)
	  contour.push_back(Point(x, y));
      }
  }
  // contour.sort();
  // contour.unique();
}



// Given region points find unordered inner contour points
// Do not include image border points
// Actual image size is controlled by imageWidth/imageHeight
// If canvas has been painted set rp = NULL
// Connect should be equal to CRCONN for consistency
void n3::getInnerContour (Points& contour, cv::Mat& canvas, 
			  Points const* rp, int connect, 
			  int imageWidth, int imageHeight, 
			  int xoff, int yoff, bool resetCanvas)
{
  if (resetCanvas) canvas.setTo(cv::Scalar(0));
  if (rp != NULL) 
    for (Points::const_iterator it = rp->begin(); it != rp->end(); 
	 ++it) canvas.at<uchar>(it->y + yoff, it->x + xoff) = 1;
  for (Points::const_iterator pit = rp->begin(); 
       pit != rp->end(); ++pit) {
    Point p(pit->x + xoff, pit->y + yoff);
    std::vector<Pixel<uchar> > n = getNeighbors(p, canvas, connect);
    for (std::vector<Pixel<uchar> >::const_iterator nit = n.begin(); 
	 nit != n.end(); ++nit)
      if (nit->val == 0) {
	float x = nit->x - xoff, y = nit->y - yoff;
	if (x >= 0 && x < imageWidth && y >= 0 && y < imageHeight)
	    contour.push_back(*pit);
	    break;
      }
  }
}



// Given unordered contours find unordered boundary points they share
void n3::getBoundary (Points& boundary, Points const& contour0, 
		      Points const& contour1, cv::Mat& canvas)
{
  canvas.setTo(cv::Scalar(0));
  for (Points::const_iterator it = contour0.begin(); 
       it != contour0.end(); ++it) canvas.at<uchar>(it->y, it->x) = 1;
  for (Points::const_iterator it = contour1.begin(); 
       it != contour1.end(); ++it) 
    if (canvas.at<uchar>(it->y, it->x) == 1) boundary.push_back(*it);
}



// If paintBoundary = false must give ub
void n3::getBoundary (std::list<Points>& ob, 
		      std::list<Points::const_iterator>& head, 
		      std::list<Points::const_iterator>& tail, 
		      Points const& oc, cv::Mat& canvas, const Points* ub)
{
  if (ub != NULL) {
    canvas.setTo(cv::Scalar(0));
    for (Points::const_iterator it = ub->begin(); it != ub->end(); ++it)
      canvas.at<uchar>(it->y, it->x) = 1;
  }
  bool start = false;
  const Points::const_iterator cst = oc.begin(); // Starting point
  Points::const_iterator pit = cst;
  while (true) {
    if (canvas.at<uchar>(pit->y, pit->x) == 1) {
      if (!start) {
	start = true;
	ob.push_back(Points());
	head.push_back(pit);
      }
      ob.back().push_back(*pit);
    }
    else if (start) {
      start = false;
      tail.push_back(oc.previous(pit, true));
    }
    pit = oc.next(pit, true);
    if (pit == cst) {
      // Last point is on boundary
      if (start) tail.push_back(oc.last());
      // End loop
      break;
    }
  }
  // Merge first piece to last piece if necessary
  if (ob.size() > 1 && head.front() == cst && tail.back() == oc.last()) {
    splice(ob.back(), ob.front(), false);
    tail.back() = tail.front();
    ob.pop_front();
    head.pop_front();
    tail.pop_front();
  }
}



// DO NOT assume all boundary pieces must be contained within one contour
// (Because it is not THE case)
// Always pick max overlapped contours
// bci0/1: boundary containing contour index
void n3::getBoundary (int& bci0, int& bci1, 
		      std::vector<Points>& ob0, std::vector<Points>& ob1, 
		      std::vector<Points::const_iterator>& head0, 
		      std::vector<Points::const_iterator>& head1, 
		      std::vector<Points::const_iterator>& tail0, 
		      std::vector<Points::const_iterator>& tail1, 
		      Points const& ub, std::vector<Points> const& oc0, 
		      std::vector<Points> const& oc1, cv::Mat& canvas)
{
  // Find boundary containing contour indices
  bci0 = -1;
  bci1 = -1;
  int bco0 = 0, bco1 = 0;
  canvas.setTo(cv::Scalar(0));
  for (Points::const_iterator it = ub.begin(); it != ub.end(); ++it)
    canvas.at<uchar>(it->y, it->x) = 1;
  for (int i = 0; i < oc0.size(); ++i) {
    int o = 0;
    for (Points::const_iterator it = oc0[i].begin(); it != oc0[i].end(); 
	 ++it) if (canvas.at<uchar>(it->y, it->x) == 1) ++o;
    if (o > bco0) {
      bci0 = i;
      bco0 = o;
    }
  }
  for (int i = 0; i < oc1.size(); ++i) {
    int o = 0;
    for (Points::const_iterator it = oc1[i].begin(); it != oc1[i].end(); 
	 ++it) if (canvas.at<uchar>(it->y, it->x) == 1) ++o;
    if (o > bco1) {
      bci1 = i;
      bco1 = o;
    }
  }
  if (bci0 < 0 || bci1 < 0)
    perr("Unexpected: cannot match boundary to contour...");
  // Get boundary pieces
  std::list<Points> tob0, tob1;
  std::list<Points::const_iterator> thead0, thead1, ttail0, ttail1;
  getBoundary(tob0, thead0, ttail0, oc0[bci0], canvas, NULL);
  getBoundary(tob1, thead1, ttail1, oc1[bci1], canvas, NULL);
  // Collect outputs
  ob0.reserve(tob0.size());
  ob1.reserve(tob1.size());
  head0.reserve(thead0.size());
  head1.reserve(thead1.size());
  tail0.reserve(ttail0.size());
  tail1.reserve(ttail1.size());
  for (std::list<Points>::iterator it = tob0.begin(); 
       it != tob0.end(); ++it) {
    ob0.push_back(Points());
    splice(ob0.back(), *it, false);
  }
  for (std::list<Points>::iterator it = tob1.begin(); 
       it != tob1.end(); ++it) {
    ob1.push_back(Points());
    splice(ob1.back(), *it, false);
  }
  for (std::list<Points::const_iterator>::const_iterator it = 
	 thead0.begin(); it != thead0.end(); ++it)
    head0.push_back(*it);
  for (std::list<Points::const_iterator>::const_iterator it = 
	 thead1.begin(); it != thead1.end(); ++it)
    head1.push_back(*it);
  for (std::list<Points::const_iterator>::const_iterator it = 
	 ttail0.begin(); it != ttail0.end(); ++it)
    tail0.push_back(*it);
  for (std::list<Points::const_iterator>::const_iterator it = 
	 ttail1.begin(); it != ttail1.end(); ++it)
    tail1.push_back(*it);
}
