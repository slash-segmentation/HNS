#ifndef _n3_bignum_h_
#define _n3_bignum_h_

#include "n3_base.h"
#include "gmpxx.h"

namespace n3 {

  const unsigned int DEFAULT_BIG_FLOAT_PREC = 1024;

  typedef mpz_class BigInt;
  typedef mpf_class BigFloat;
  
};

#endif
