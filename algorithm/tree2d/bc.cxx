#include "bc.h"
using namespace n3;
using namespace n3::tree2d;

void n3::tree2d::getUnorderedGeometryFeatures 
(flist& feat, Points const* r0, Points const* r1, Points const* uc0, 
 Points const* uc1, Points const* ub, bool swap01)
{
  if (swap01) {
    std::swap(r0, r1);
    std::swap(uc0, uc1);
  }
  float a0 = r0->size(), a1 = r1->size();
  compute(feat, a0, a1);
  float p0 = uc0->size(), p1 = uc1->size(), b = ub->size();
  compute(feat, p0, p1);
  feat.push_back(b);
  feat.push_back(getr(b, p0));
  feat.push_back(getr(b, p1));
  feat.push_back(getr(p0 * p0, a0));
  feat.push_back(getr(p1 * p1, a1));
}


void n3::tree2d::getIntensityFeatures (flist& feat, Points const& p, 
				       FloatImage::Pointer valImage, 
				       float histLower, float histUpper, 
				       int histBin)
{
  flist v; 
  getvs<FloatImage>(v, valImage, p);
  append(feat, getHist(v, histLower, histUpper, histBin, true));
  float mean = getMean(v);
  feat.push_back(getMin(v));
  feat.push_back(getMax(v));
  feat.push_back(mean);
  feat.push_back(getMedian(v));
  feat.push_back(getStd(v, mean));
}



void n3::tree2d::getIntensityFeatures (flist& feat, Points const& p0, 
				       Points const& p1, 
				       FloatImage::Pointer valImage, 
				       float histLower, float histUpper, 
				       int histBin)
{
  flist v0, v1;
  getvs<FloatImage>(v0, valImage, p0);
  getvs<FloatImage>(v1, valImage, p1);
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



void n3::tree2d::getTextonFeatures (flist& feat, Points const& p0, 
				    Points const& p1, 
				    FloatImage::Pointer valImage, 
				    TextonDict const& tdict, 
				    LabelImage::Pointer canvas)
{
  fvec h0, h1;
  genTextonHist(h0, tdict, p0, valImage, canvas, true);
  genTextonHist(h1, tdict, p1, valImage, canvas, true);
  feat.push_back(getHistDistL1(h0, h1));
  feat.push_back(getHistDistX2(h0, h1));
}



// Faster texton feature generation
void n3::tree2d::getTextonFeatures 
(flist& feat, Points const& p0, Points const& p1, TextonDict const& tdict, 
 std::vector<std::vector<TextonPatch> > const& textons, 
 UInt8Image::Pointer check, LabelImage::Pointer canvas)
{
  fvec h0, h1;
  genTextonHist(h0, tdict, textons, p0, check, canvas, false);
  genTextonHist(h1, tdict, textons, p1, check, canvas, false);
  feat.push_back(getHistDistL1(h0, h1));
  feat.push_back(getHistDistX2(h0, h1));
}



void n3::tree2d::getSaliencyFeatures (flist& feat, float saliency0, 
				      float saliency1, float saliency2, 
				      bool swap01)
{
  if (swap01) std::swap(saliency0, saliency1);
  feat.push_back(saliency0);
  feat.push_back(saliency1);
  feat.push_back(saliency2);
  float ds0 = saliency2 - saliency0;
  float ds1 = saliency2 - saliency1;
  feat.push_back(ds0);
  feat.push_back(ds1);
  feat.push_back(fabs(ds0 - ds1));
}
