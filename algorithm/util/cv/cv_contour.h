#ifndef _n3_cv_contour_h_
#define _n3_cv_contour_h_

#include "n3_util.h"
#include "struct/point.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace n3 {

  /* Connect should be 4 or 8 */
  /* If returned vector is not full, point must be on image border */
  std::vector<Pixel<uchar> > 
    getNeighbors (Point const& p, cv::Mat const& im, int connect);

  /* Transform Points to a vector of cv::Point2f */
  void transform (std::vector<cv::Point2f>& cvp, Points const& p);

  // Get ordered contours of a region
  // Only consider contour points (cp) that are 4-adjacent to region (rp)
  // First contour is outmost
  // Set xoff/yoff >= 1 to include image border points 
  // Optionally paint rp/cp by giving non-NULL pointers
  // Only when resetCanvas == true canvas is set to 0
  void getContour (std::vector<Points>& contour, cv::Mat& canvas, 
		   Points const* rp, Points const* cp, 
		   int xoff, int yoff, bool resetCanvas);

  // Given region points find unordered outer contour points
  // Do not include image border points
  // Actual image size is controlled by imageWidth/imageHeight
  // If canvas has been painted set rp = NULL
  // Connect should be equal to CRCONN for consistency
  void getContour (Points& contour, cv::Mat& canvas, Points const* rp, 
		   int connect, int imageWidth, int imageHeight, 
		   int xoff, int yoff, bool resetCanvas);

  // Given region points find unordered inner contour points
  // Do not include image border points
  // Actual image size is controlled by imageWidth/imageHeight
  // If canvas has been painted set rp = NULL
  // Connect should be equal to CRCONN for consistency
  void getInnerContour (Points& contour, cv::Mat& canvas, 
			Points const* rp, int connect, 
			int imageWidth, int imageHeight, 
			int xoff, int yoff, bool resetCanvas);

  // Given unordered contours find unordered boundary points they share
  void getBoundary (Points& boundary, Points const& contour0,
  		    Points const& contour1, cv::Mat& canvas);
  
  // ob: output ordered boundary on each contour
  // head: ordered boundary head point on ordered contour
  // tail: ordered boundary tail point on ordered contour
  // ub: input unordered boundary
  // oc: input ordered contour
  // If paintBoundary = false must give ub
  void getBoundary (std::list<Points>& ob, 
		    std::list<Points::const_iterator>& head, 
		    std::list<Points::const_iterator>& tail, 
		    Points const& oc, cv::Mat& canvas, 
		    const Points* ub = NULL);

  // Assume all boundary pieces must be contained within one contour
  // bci0/1: boundary containing contour index
  // ob0/1: output ordered boundary on each contour
  // head0/1: ordered boundary head point on ordered contour
  // tail0/1: ordered boundary tail point on ordered contour
  // ub: input unordered boundary
  // oc0/1: input ordered contour
  void getBoundary (int& bci0, int& bci1, 
		    std::vector<Points>& ob0, std::vector<Points>& ob1, 
		    std::vector<Points::const_iterator>& head0, 
		    std::vector<Points::const_iterator>& head1, 
		    std::vector<Points::const_iterator>& tail0, 
		    std::vector<Points::const_iterator>& tail1, 
		    Points const& ub, std::vector<Points> const& oc0, 
		    std::vector<Points> const& oc1, cv::Mat& canvas);

  /* Return if there is any point with label in p's 4-neighbor */
  inline bool adj4 (Point const& p, int xoff, int yoff, int label, 
		    cv::Mat const& canvas)
  {
    int w = canvas.cols, h = canvas.rows;
    int x = p.x + xoff, y = p.y + yoff;
    if (x > 0 && canvas.at<uchar>(y, x - 1) == label || 
	x < w - 1 && canvas.at<uchar>(y, x + 1) == label || 
	y > 0 && canvas.at<uchar>(y - 1, x) == label || 
	y < h - 1 && canvas.at<uchar>(y + 1, x) == label) return true;
    return false;
  }

 
  /* Transform Points to a vector of TPoint */
  template <typename TPoint>
    void transform (std::vector<TPoint>& cvp, Points const& p)
    {
      cvp.reserve(p.size());
      for (Points::const_iterator it = p.begin(); it != p.end(); ++it)
	cvp.push_back(TPoint(it->x, it->y));
    }

};

#endif
