#ifndef _n3_cv_hist_h_
#define _n3_cv_hist_h_

#include "struct/point.h"

namespace n3 {
  
  double getHistDistL1 (fvec const& h0, fvec const& h1);
  double getHistDistX2 (fvec const& h0, fvec const& h1);
  double getHistDistJD (fvec const& h0, fvec const& h1);

};

#endif
