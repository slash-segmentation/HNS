#include "cv_hist.h"
using namespace n3;

void n3::getHistAbsDiff (flist& hdiff, fvec const& h0, fvec const& h1)
{
  fvec::const_iterator hit0 = h0.begin(), hit1 = h1.begin();
  while (hit0 != h0.end()) {
    hdiff.push_back(fabs(*hit0 - *hit1));
    ++hit0;
    ++hit1;
  }
}



double n3::getHistDistL1 (fvec const& h0, fvec const& h1)
{
  if (h0.size() != h1.size()) 
    perr("Error: histogram dimensions disagree...");
  double ret = 0.0;
  fvec::const_iterator it0 = h0.begin(), it1 = h1.begin();
  while (it0 != h0.end()) {
    ret += fabs(*it0 - *it1);
    ++it0;
    ++it1;
  }
  return ret;
}



double n3::getHistDistX2 (fvec const& h0, fvec const& h1)
{
  if (h0.size() != h1.size()) 
    perr("Error: histogram dimensions disagree...");
  double ret = 0.0;
  fvec::const_iterator it0 = h0.begin(), it1 = h1.begin();
  while (it0 != h0.end()) {
    double x0 = *it0 + EPSILON, x1 = *it1 + EPSILON;
    ret += (x0 - x1) * (x0 - x1) / (x0 + x1);
    ++it0;
    ++it1;
  }
  return ret;
}




double n3::getHistDistJD (fvec const& h0, fvec const& h1)
{
  if (h0.size() != h1.size()) 
    perr("Error: histogram dimensions disagree...");
  double ret = 0.0;
  fvec::const_iterator it0 = h0.begin(), it1 = h1.begin();
  while (it0 != h0.end()) {
    double x0 = *it0 + EPSILON, x1 = *it1 + EPSILON;
    double avg = (x0 + x1) / 2.0;
    ret += x0 * log(x0 / avg) + x1 * log(x1 / avg);
    ++it0;
    ++it1;
  }
  return ret;
}
