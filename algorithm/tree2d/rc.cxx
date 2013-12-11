#include "rc.h"
using namespace n3;
using namespace n3::tree2d;

void getShape (flist& feat, Points const& region, Points const& contour, 
	       cv::Mat& canvas)
{
  std::vector<Points> ocontour;
  getContour(ocontour, canvas, &region, &contour, 1, 1, true);
  int width = canvas.cols - 2, height = canvas.rows - 2;
  bool border = false;
  for (Points::const_iterator pit = ocontour[0].begin(); 
       pit != ocontour[0].end(); ++pit) 
    if (pit->x == 0 || pit->x == width - 1 || 
	pit->y == 0 || pit->y == height - 1) {
      border = true;
      break;
    }
  feat.push_back(border? 1.0: 0.0);
  float area = region.size(), perimeter = getLength(ocontour, true);
  feat.push_back(area);
  feat.push_back(perimeter);
  feat.push_back(getr(perimeter * perimeter, area));
  std::vector<cv::Point> _outer;
  transform(_outer, ocontour[0]);
  cv::Moments mu = cv::moments(_outer, false);
  double hu[7];
  cv::HuMoments(mu, hu);
  for (int i = 0; i < 7; ++i) feat.push_back(hu[i]);
  std::vector<int> convHull;
  cv::convexHull(_outer, convHull);
  std::vector<cv::Vec4i> convDefects;
  cv::convexityDefects(_outer, convHull, convDefects);
  std::vector<float> convDefectDepths(convDefects.size());
  for (int i = 0; i < convDefects.size(); ++i)
    convDefectDepths[i] = (float)convDefects[i][3] / 256.0;
  feat.push_back(getMin(convDefectDepths));
  feat.push_back(getMax(convDefectDepths));
  float convDefectDepthMean = getMean(convDefectDepths);
  feat.push_back(convDefectDepthMean);
  feat.push_back(getMedian(convDefectDepths));
  feat.push_back(getStd(convDefectDepths, convDefectDepthMean));
  if (_outer.size() >= 5) {
    cv::RotatedRect ellipseBox = cv::fitEllipse(_outer);
    feat.push_back(ellipseBox.size.width * ellipseBox.size.height);
    feat.push_back(ellipseBox.size.width);
    feat.push_back(ellipseBox.size.height);
    feat.push_back(getr(ellipseBox.size.width, ellipseBox.size.height));
  }
  else {
    feat.push_back(DUMMY);
    feat.push_back(DUMMY);
    feat.push_back(DUMMY);
    feat.push_back(DUMMY);
  }
  cv::RotatedRect minAreaBox = cv::minAreaRect(_outer);
  feat.push_back(minAreaBox.size.width * minAreaBox.size.height);
  feat.push_back(minAreaBox.size.width);
  feat.push_back(minAreaBox.size.height);
  feat.push_back(getr(minAreaBox.size.width, minAreaBox.size.height));
  float minEnclosingCircleRadius;
  cv::Point2f minEnclosingCircleCenter;
  cv::minEnclosingCircle(_outer, minEnclosingCircleCenter, 
			 minEnclosingCircleRadius);
  feat.push_back(minEnclosingCircleRadius);
  feat.push_back(ocontour.size());
  flist internalAngles;
  Points vertices;
  approxCurve(vertices, ocontour[0], 1, true); // Only for outer contour
  getInternalAngles(internalAngles, vertices, true);
  append(feat, getHist(internalAngles, 0.0, 2.0 * PI, 
		       RC_IA_HIST_BIN, true));
  feat.push_back(getMin(internalAngles));
  feat.push_back(getMax(internalAngles));
  float internalAngleMean = getMean(internalAngles);
  feat.push_back(internalAngleMean);
  feat.push_back(getMedian(internalAngles));
  feat.push_back(getStd(internalAngles, internalAngleMean));  
}



void getIntensity (flist& feat, Points const& contour, 
		   FloatImage::Pointer valImage)
{
  std::list<Float> vals;  
  getvs<FloatImage>(vals, valImage, contour);
  append(feat, getHist(vals, 0.0, 1.0, RC_RI_HIST_BIN, true));
  feat.push_back(getMin(vals));
  feat.push_back(getMax(vals));
  float vmean = getMean(vals);
  feat.push_back(vmean);
  feat.push_back(getMedian(vals));
  feat.push_back(getStd(vals, vmean));
}



void n3::tree2d::genRegionFeature 
(flist& feat, Points const& region, Points& contour, 
 FloatImage::Pointer rawImage, cv::Mat& canvas, 
 bool useIntensity)
{
  getShape(feat, region, contour, canvas);
  if (useIntensity) getIntensity(feat, contour, rawImage);
}
