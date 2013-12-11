#include "bc.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

void n3::tree3d::getUnorderedGeometryFeatures 
(flist& feat, Points3 const* r0, Points3 const* r1, Points3 const* uc0, 
 Points3 const* uc1, Points3 const* ub, bool swap01)
{
  if (swap01) {
    std::swap(r0, r1);
    std::swap(uc0, uc1);
  }
  float v0 = r0->size(), v1 = r1->size();
  tree2d::compute(feat, v0, v1);
  float a0 = uc0->size(), a1 = uc1->size(), b = ub->size();
  tree2d::compute(feat, a0, a1);
  feat.push_back(b);
  feat.push_back(getr(b, a0));
  feat.push_back(getr(b, a1));
  feat.push_back(getr(pow(a0, 1.5), v0));
  feat.push_back(getr(pow(a1, 1.5), v1));
}



void n3::tree3d::getIntensityFeatures (flist& feat, Points3 const& p, 
				       FloatImage3::Pointer valImage, 
				       float histLower, float histUpper, 
				       int histBin)
{
  flist v; 
  getvs<FloatImage3>(v, valImage, p);
  append(feat, getHist(v, histLower, histUpper, histBin, true));
  float mean = getMean(v);
  feat.push_back(getMin(v));
  feat.push_back(getMax(v));
  feat.push_back(mean);
  feat.push_back(getMedian(v));
  feat.push_back(getStd(v, mean));
}



void n3::tree3d::getIntensityFeatures (flist& feat, Points3 const& p0, 
				       Points3 const& p1, 
				       FloatImage3::Pointer valImage, 
				       float histLower, float histUpper, 
				       int histBin)
{
  flist v0, v1;
  getvs<FloatImage3>(v0, valImage, p0);
  getvs<FloatImage3>(v1, valImage, p1);
  fvec h0 = getHist(v0, histLower, histUpper, histBin, true); 
  fvec h1 = getHist(v1, histLower, histUpper, histBin, true);
  feat.push_back(getHistDistL1(h0, h1));
  feat.push_back(getHistDistX2(h0, h1));
  float mean0 = getMean(v0), mean1 = getMean(v1);
  feat.push_back(fabs(getMin(v0) - getMin(v1)));
  feat.push_back(fabs(getMax(v0) - getMax(v1)));
  feat.push_back(fabs(mean0 - mean1));
  feat.push_back(fabs(getMedian(v0) - getMedian(v1)));
  feat.push_back(fabs(getStd(v0, mean0) - getStd(v1, mean1)));
}



void n3::tree3d::getTextonFeatures (flist& feat, Points3 const& p0, 
				    Points3 const& p1, 
				    FloatImage3::Pointer valImage, 
				    TextonDict const& tdict, 
				    LabelImage3::Pointer canvas)
{
  fvec h0, h1;
  genTextonHist(h0, tdict, p0, valImage, canvas, false);
  genTextonHist(h1, tdict, p1, valImage, canvas, false);
  feat.push_back(getHistDistL1(h0, h1));
  feat.push_back(getHistDistX2(h0, h1));
}
