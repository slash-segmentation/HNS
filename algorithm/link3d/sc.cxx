#include "sc.h"
using namespace n3;
using namespace n3::link3d;

void n3::link3d::getSimpleGeometry (flist& feat, double a0, double a1, 
				    double ap0, double ap1, double cd, 
				    double overlap, bool swap01)
{
  if (swap01) {
    std::swap(a0, a1);
    std::swap(ap0, ap1);
  }
  compute(feat, a0, a1);
  feat.push_back(overlap);
  feat.push_back(getr(overlap, a0));
  feat.push_back(getr(overlap, a1));
  double sd = a0 + a1 - 2.0 * overlap;
  feat.push_back(sd);
  feat.push_back(getr(sd, (a0 + a1 - overlap)));
  feat.push_back(getr(sd, a0));
  feat.push_back(getr(sd, a1));
  compute(feat, ap0, ap1);
  feat.push_back(cd);
  feat.push_back(getr(cd, ap0));
  feat.push_back(getr(cd, ap1));
  feat.push_back(getr(cd, ap0 + ap1));
  feat.push_back(getr(cd, sqrt(a0)));
  feat.push_back(getr(cd, sqrt(a1)));
  feat.push_back(getr(cd, sqrt(a0) + sqrt(a1)));
  double acp0 = getr(ap0 * ap0, a0), acp1 = getr(ap1 * ap1, a1);
  feat.push_back(acp0);
  feat.push_back(acp1);
  feat.push_back(fabs(acp0 - acp1));
}



void n3::link3d::getAdvancedGeometry (flist& feat, RegionStat const* rs0, 
				      RegionStat const* rs1, bool swap01)
{
  if (swap01) std::swap(rs0, rs1);
  feat.push_back(rs0->border? 1: 0);
  feat.push_back(rs1->border? 1: 0);
  double a0 = rs0->stats[0][0], a1 = rs1->stats[0][0];
  double p0 = rs0->stats[1][0], p1 = rs1->stats[1][0];
  compute(feat, p0, p1);
  double cd = rs0->centroid.get_distance(rs1->centroid);
  feat.push_back(getr(cd, p0));
  feat.push_back(getr(cd, p1));
  feat.push_back(getr(cd, p0 + p1));
  double cp0 = rs0->stats[2][0], cp1 = rs1->stats[2][0];
  feat.push_back(cp0);
  feat.push_back(cp1);
  feat.push_back(fabs(cp0 - cp1));
  append(feat, rs0->stats[3]);
  append(feat, rs1->stats[3]);
  append(feat, abs(subtract(rs0->stats[3], rs1->stats[3])));
  std::vector<cv::Point> outer0, outer1;
  transform(outer0, rs0->ocontour[0]);
  transform(outer1, rs1->ocontour[0]);
  feat.push_back(cv::matchShapes(outer0, outer1, CV_CONTOURS_MATCH_I1, 0));
  feat.push_back(cv::matchShapes(outer0, outer1, CV_CONTOURS_MATCH_I2, 0));
  feat.push_back(cv::matchShapes(outer0, outer1, CV_CONTOURS_MATCH_I3, 0));
  compute(feat, rs0->stats[5], rs1->stats[5]);
  compute(feat, rs0->box.width, rs0->box.height, rs1->box.width, 
	  rs1->box.height, rs0->stats[0][0], rs1->stats[0][0]);
  compute(feat, rs0->stats[6][0], rs0->stats[6][1], rs1->stats[6][0], 
	  rs1->stats[6][1], rs0->stats[0][0], rs1->stats[0][0]);
  compute(feat, rs0->stats[7][0], rs0->stats[7][1], rs1->stats[7][0], 
	  rs1->stats[7][1], rs0->stats[0][0], rs1->stats[0][0]);
  double mecr0 = rs0->stats[8][0], mecr1 = rs1->stats[8][0];
  feat.push_back(mecr0);
  feat.push_back(mecr1);
  feat.push_back(fabs(mecr0 - mecr1));
  feat.push_back(getr(mecr0, mecr1));
  double meca0 = PI * mecr0 * mecr0, meca1 = PI * mecr1 * mecr1;
  feat.push_back(meca0);
  feat.push_back(meca1);
  feat.push_back(fabs(meca0 - meca1));
  feat.push_back(getr(meca0, meca1));
  double mecf0 = getr(a0, meca0), mecf1 = getr(a1, meca1);
  feat.push_back(mecf0);
  feat.push_back(mecf1);
  feat.push_back(fabs(mecf0 - mecf1));
  feat.push_back(rs0->ocontour.size());
  feat.push_back(rs1->ocontour.size());
  feat.push_back(fabs(rs0->ocontour.size() - rs1->ocontour.size()));
  append(feat, rs0->stats[10]);
  append(feat, rs1->stats[10]);
  append(feat, abs(subtract(rs0->stats[10], rs1->stats[10])));
  compute(feat, rs0->stats[11], rs1->stats[11]);
}
