#include "rs.h"
using namespace n3;
using namespace n3::link3d;

// Gather region points
void n3::link3d::getRegionPoints (std::map<Label, RegionStat>& smap, 
				  LabelImage::Pointer labelImage)
{
  for (itk::ImageRegionConstIteratorWithIndex<LabelImage> 
	 it(labelImage, labelImage->GetLargestPossibleRegion()); 
       !it.IsAtEnd(); ++it)
    if (it.Get() != BGVAL) 
      smap[it.Get()].points.push_back(i2p(it.GetIndex()));
}



// Region and unordered contour points should have been initialized
void n3::link3d::getRegionStat (RegionStat& rs, cv::Mat& canvas, 
				int imageWidth, int imageHeight)
{
  // Get contours
  getContour(rs.ocontour, canvas, &rs.points, &rs.ucontour, 1, 1, true);
  // Determine whether region touches image borders
  rs.border = false;
  for (Points::const_iterator pit = rs.ocontour[0].begin(); 
       pit != rs.ocontour[0].end(); ++pit)
    if (pit->x == 0 || pit->x == imageWidth - 1 || 
	pit->y == 0 || pit->y == imageHeight - 1) {
      rs.border = true;
      break;
    }
  // Compute polygons and other stuff
  rs.polygon.reserve(rs.ocontour.size());
  for (std::vector<Points>::const_iterator cit = rs.ocontour.begin(); 
       cit != rs.ocontour.end(); ++cit) {
    rs.polygon.push_back(Points());
    approxCurve(rs.polygon.back(), *cit, 2.0, true);
  }
  rs.centroid = rs.points.get_centroid();
  rs.box = rs.points.get_box();
  // Compute stats
  rs.stats.reserve(12);
  float area = (float)rs.points.size();
  float perimeter = getLength(rs.ocontour, true);
  rs.stats.push_back(fvec(1, area));
  rs.stats.push_back(fvec(1, perimeter));
  rs.stats.push_back(fvec(1, perimeter * perimeter / area));
  std::vector<cv::Point> outer;    
  transform(outer, rs.ocontour[0]);
  cv::Moments mu = cv::moments(outer, false);
  double hu[7];
  cv::HuMoments(mu, hu);
  rs.stats.push_back(fvec());
  rs.stats.back().reserve(7);
  for (int i = 0; i < 7; ++i) rs.stats.back().push_back(hu[i]);
  std::vector<int> chull;
  cv::convexHull(outer, chull);
  std::vector<cv::Vec4i> cdefects;
  cv::convexityDefects(outer, chull, cdefects);
  rs.stats.push_back(fvec(cdefects.size()));
  for (int i = 0; i < cdefects.size(); ++i) 
    rs.stats.back()[i] = cdefects[i][3] / 256.0;
  fvec tvec(5, DUMMY);
  tvec[0] = getMin(rs.stats.back());
  tvec[1] = getMax(rs.stats.back());
  tvec[2] = getMean(rs.stats.back());
  tvec[3] = getMedian(rs.stats.back());
  tvec[4] = getStd(rs.stats.back(), tvec[2]);
  rs.stats.push_back(tvec);
  rs.stats.push_back(fvec(2, DUMMY));
  if (outer.size() >= 5) {
    cv::RotatedRect ebox = cv::fitEllipse(outer);
    rs.stats.back()[0] = ebox.size.width;
    rs.stats.back()[1] = ebox.size.height;
  }
  rs.stats.push_back(fvec(2, DUMMY));
  cv::RotatedRect mabox = cv::minAreaRect(outer);
  rs.stats.back()[0] = mabox.size.width;
  rs.stats.back()[1] = mabox.size.height;
  rs.stats.push_back(fvec(1, DUMMY));
  cv::Point2f meccenter;
  cv::minEnclosingCircle(outer, meccenter, rs.stats.back()[0]);
  tvec.clear();
  tvec.reserve(rs.polygon[0].size());
  getInternalAngles(tvec, rs.polygon[0], true);
  rs.stats.push_back(tvec);
  rs.stats.push_back(getHist(tvec, 0.0, 2.0 * PI, 
			      RS_IA_HIST_BIN, true));
  rs.stats.push_back(fvec(5, DUMMY));
  rs.stats.back()[0] = getMin(tvec);
  rs.stats.back()[1] = getMax(tvec);
  rs.stats.back()[2] = getMean(tvec);
  rs.stats.back()[3] = getMedian(tvec);
  rs.stats.back()[4] = getStd(tvec, rs.stats.back()[2]);
}
