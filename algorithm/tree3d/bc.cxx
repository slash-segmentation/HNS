#include "bc.h"
using namespace n3;
using namespace n3::tree2d;
using namespace n3::tree3d;

// b0/b1: image border points, use NULL if none
void n3::tree3d::getBoundaryUnorderedGeometryFeatures 
(flist& feat, Points3 const* r0, Points3 const* r1, Points3 const* uc0, 
 Points3 const* uc1, Points3 const* ub, Points3 const* b0, 
 Points3 const* b1, bool swap01)
{
  if (swap01) {
    std::swap(r0, r1);
    std::swap(uc0, uc1);
    std::swap(b0, b1);
  }
  feat.push_back(b0 == NULL? 0.0: 1.0);
  feat.push_back(b1 == NULL? 0.0: 1.0);
  float v0 = r0->size(), v1 = r1->size();
  tree2d::compute(feat, v0, v1);
  float a0 = uc0->size(), a1 = uc1->size(), b = ub->size();
  if (b0 != NULL) a0 += b0->size();
  if (b1 != NULL) a1 += b1->size();
  tree2d::compute(feat, a0, a1);
  feat.push_back(b);
  feat.push_back(getr(b, a0));
  feat.push_back(getr(b, a1));
  feat.push_back(getr(pow(a0, 1.5), v0));
  feat.push_back(getr(pow(a1, 1.5), v1));
}



void n3::tree3d::getBoundaryIntensityFeatures 
(flist& feat, Points3 const& p, FloatImage3::Pointer valImage, 
 float histLower, float histUpper, int histBin)
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



void n3::tree3d::getBoundaryIntensityFeatures 
(flist& feat, Points3 const& p0, Points3 const& p1, 
 FloatImage3::Pointer valImage, float histLower, float histUpper, 
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



void n3::tree3d::getBoundaryTextonFeatures (flist& feat, Points3 const& p0, 
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



// Does not include saliency features
void n3::tree3d::getBoundaryFeatures (flist& feat, Points3 const* r0, 
				      Points3 const* r1, 
				      Points3 const* uc0, 
				      Points3 const* uc1, 
				      Points3 const* b0, 
				      Points3 const* b1, 
				      LabelImage3::Pointer canvas, 
				      FloatImage3::Pointer rawImage, 
				      FloatImage3::Pointer pbImage)
{
  Points3 ub;
  getBoundary(ub, *uc0, *uc1, canvas);
  bool swap01 = r0->size() > r1->size();
  getBoundaryUnorderedGeometryFeatures(feat, r0, r1, uc0, uc1, &ub, 
				       b0, b1, swap01);
  getBoundaryIntensityFeatures(feat, ub, rawImage, 
			       0.0, 1.0, BC_RI_HIST_BIN);
  getBoundaryIntensityFeatures(feat, ub, pbImage, 
			       0.0, 1.0, BC_PB_HIST_BIN);
}
