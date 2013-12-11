#include "util/stat_util.h"
#include "cv_texton.h"
using namespace n3;

bool isInside (Point const& p, int halfSize, LabelImage::Pointer canvas)
{
  const int w = getw<LabelImage>(canvas), h = geth<LabelImage>(canvas);
  if (p.x - halfSize < 0 || p.x + halfSize >= w || 
      p.y - halfSize < 0 || p.y + halfSize >= h)
    return false;
  for (int dx = -halfSize; dx <= halfSize; ++dx)
    for (int dy = -halfSize; dy <= halfSize; ++dy) 
      if (getv<LabelImage>(canvas, p.x + dx, p.y + dy) == 0) return false;
  return true;
}



bool isInside (Point3 const& p, int halfSize, LabelImage3::Pointer canvas)
{
  const int w = getw<LabelImage3>(canvas), h = geth<LabelImage3>(canvas), 
    d = getd<LabelImage3>(canvas);
  if (p.x - halfSize < 0 || p.x + halfSize >= w || 
      p.y - halfSize < 0 || p.y + halfSize >= h || 
      p.z - halfSize < 0 || p.z + halfSize >= d)
    return false;
  for (int dx = -halfSize; dx <= halfSize; ++dx)
    for (int dy = -halfSize; dy <= halfSize; ++dy) 
      for (int dz = -halfSize; dz <= halfSize; ++dz)
	if (getv<LabelImage3>(canvas, p.x + dx, p.y + dy, p.z + dz) == 0) 
	  return false;
  return true;
}



// For current use, set resetCanvas to false for speed
// Otherwise, set resetCanvas to true for safety
void n3::genTextonPatch (std::list<TextonPatch>& pats, 
			 FloatImage::Pointer valImage, 
			 Points const& points, LabelImage::Pointer canvas, 
			 bool resetCanvas)
{
  if (resetCanvas) canvas->FillBuffer(0);
  setvs<LabelImage>(canvas, points, 1);
  for (Points::const_iterator it = points.begin(); 
       it != points.end(); ++it) 
    if (isInside(*it, TEXTON_PAT_HALF_SIZE, canvas)) {
      TextonPatch pat(TEXTON_PAT_LEN, 0.0);
      int i = 0;
      for (int dx = -TEXTON_PAT_HALF_SIZE; dx <= TEXTON_PAT_HALF_SIZE; 
	   ++dx) 
	for (int dy = -TEXTON_PAT_HALF_SIZE; dy <= TEXTON_PAT_HALF_SIZE; 
	     ++dy) 
	  pat[i++] = getv<FloatImage>(valImage, it->x + dx, it->y + dy);
      pats.push_back(pat);
    }
}



// For current use, set resetCanvas to false for speed
// Otherwise, set resetCanvas to true for safety
void n3::genTextonPatch (std::list<TextonPatch>& pats, 
			 FloatImage3::Pointer valImage, 
			 Points3 const& points, LabelImage3::Pointer canvas, 
			 bool resetCanvas)
{
  if (resetCanvas) canvas->FillBuffer(0);
  setvs<LabelImage3>(canvas, points, 1);
  for (Points3::const_iterator it = points.begin(); 
       it != points.end(); ++it) 
    if (isInside(*it, TEXTON_PAT_HALF_SIZE, canvas)) {
      TextonPatch pat(TEXTON_PAT_LEN3, 0.0);
      int i = 0;
      for (int dx = -TEXTON_PAT_HALF_SIZE; dx <= TEXTON_PAT_HALF_SIZE; 
	   ++dx) 
	for (int dy = -TEXTON_PAT_HALF_SIZE; dy <= TEXTON_PAT_HALF_SIZE; 
	     ++dy) 
	  for (int dz = -TEXTON_PAT_HALF_SIZE; dz <= TEXTON_PAT_HALF_SIZE; 
	       ++dz)
	    pat[i++] = getv<FloatImage3>(valImage, it->x + dx, 
					 it->y + dy, it->z + dz);
      pats.push_back(pat);
    }
}



void n3::genTextonHist (fvec& hist, TextonDict const& dict, 
			std::list<TextonPatch> const& pats)
{
  int bin = dict.size();
  hist.resize(bin, 0.0);
  for (std::list<TextonPatch>::const_iterator pit = pats.begin(); 
       pit != pats.end(); ++pit) {
    double dist[bin], sdist = 0.0;
    for (int i = 0; i < bin; ++i) {
      dist[i] = exp(-norm(subtract(*pit, dict[i]), 2));
      sdist += dist[i];
    }
    for (int i = 0; i < bin; ++i) hist[i] += (float)(dist[i] / sdist);
  }
  double shist = getSum(hist);
  if (shist > EPSILON)
    for (int i = 0; i < bin; ++i) 
      hist[i] = (float)((double)hist[i] / shist);
}



void n3::genTextonHist (fvec& hist, TextonDict const& dict, 
			Points const& points, FloatImage::Pointer valImage, 
			LabelImage::Pointer canvas, bool resetCanvas)
{
  std::list<TextonPatch> pats;
  genTextonPatch(pats, valImage, points, canvas, resetCanvas);
  genTextonHist(hist, dict, pats);
}



void n3::genTextonHist (fvec& hist, TextonDict const& dict, 
			Points3 const& points, 
			FloatImage3::Pointer valImage, 
			LabelImage3::Pointer canvas, bool resetCanvas)
{
  std::list<TextonPatch> pats;
  genTextonPatch(pats, valImage, points, canvas, resetCanvas);
  genTextonHist(hist, dict, pats);
}


//-------------------------------------------------------------

void n3::genTextonHist (fvec& hist, TextonDict const& dict, 
			std::list<TextonPatch const*> const& pats)
{
  int bin = dict.size();
  hist.resize(bin, 0.0);
  for (std::list<TextonPatch const*>::const_iterator pit = pats.begin(); 
       pit != pats.end(); ++pit) {
    double dist[bin], sdist = 0.0;
    for (int i = 0; i < bin; ++i) {
      dist[i] = exp(-norm(subtract(**pit, dict[i]), 2));
      sdist += dist[i];
    }
    for (int i = 0; i < bin; ++i) hist[i] += (float)(dist[i] / sdist);
  }
  double shist = getSum(hist);
  if (shist > EPSILON)
    for (int i = 0; i < bin; ++i) 
      hist[i] = (float)((double)hist[i] / shist);
}



// Generate textons for every point except borders
void n3::genTextons (std::vector<std::vector<TextonPatch> >& textons, 
		     FloatImage::Pointer valImage)
{
  int w = getw<FloatImage>(valImage), h = geth<FloatImage>(valImage);
  textons.resize(w - TEXTON_PAT_HALF_SIZE * 2);
  int wh = w - TEXTON_PAT_HALF_SIZE, hh = h - TEXTON_PAT_HALF_SIZE;
  for (int x = TEXTON_PAT_HALF_SIZE; x < wh; ++x) {
    textons[x - TEXTON_PAT_HALF_SIZE]
      .resize(h - TEXTON_PAT_HALF_SIZE * 2, 
	      TextonPatch(TEXTON_PAT_LEN, 0.0));
    for (int y = TEXTON_PAT_HALF_SIZE; y < hh; ++y) {
      TextonPatch* p = 
	&(textons[x - TEXTON_PAT_HALF_SIZE][y - TEXTON_PAT_HALF_SIZE]);
      int i = 0;
      for (int dx = -TEXTON_PAT_HALF_SIZE; dx <= TEXTON_PAT_HALF_SIZE; 
	   ++dx) 
	for (int dy = -TEXTON_PAT_HALF_SIZE; dy <= TEXTON_PAT_HALF_SIZE; 
	     ++dy) 
	  (*p)[i++] = getv<FloatImage>(valImage, x + dx, y + dy);
    }
  }
}



// For current use, set resetCanvas to false for speed
// Otherwise, set resetCanvas to true for safety
void n3::genTextonPatch (std::list<TextonPatch const*>& pats, 
			 std::vector<std::vector<TextonPatch> > const& 
			 textons, Points const& points, 
			 UInt8Image::Pointer check, 
			 LabelImage::Pointer canvas, bool resetCanvas)
{
  bool canvasSet = false;
  for (Points::const_iterator pit = points.begin(); pit != points.end(); 
       ++pit) {
    if (getv<UInt8Image>(check, *pit) == 1)
      pats.push_back(&(textons[pit->x - TEXTON_PAT_HALF_SIZE]
		       [pit->y - TEXTON_PAT_HALF_SIZE]));
    else {
      if (!canvasSet) {
	if (resetCanvas) canvas->FillBuffer(0);
	setvs<LabelImage>(canvas, points, 1);
	canvasSet = true;
      }
      if (isInside(*pit, TEXTON_PAT_HALF_SIZE, canvas)) {
	setv<UInt8Image>(check, *pit, 1);
	pats.push_back(&(textons[pit->x - TEXTON_PAT_HALF_SIZE]
			 [pit->y - TEXTON_PAT_HALF_SIZE]));
      }
    }
  }
}



// For current use, set resetCanvas to false for speed
// Otherwise, set resetCanvas to true for safety
void n3::genTextonHist (fvec& hist, TextonDict const& dict, 
			std::vector<std::vector<TextonPatch> > const& 
			textons, Points const& points, 
			UInt8Image::Pointer check, 
			LabelImage::Pointer canvas, bool resetCanvas)
{
  std::list<TextonPatch const*> pats;
  genTextonPatch(pats, textons, points, check, canvas, resetCanvas);
  genTextonHist(hist, dict, pats);
}
