#ifndef _n3_base_h_
#define _n3_base_h_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector> 
#include <list> 
#include <map> 
#include <set> 
#include <queue>
#include <iterator> 
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cmath>
#include <cfloat>
#include <climits>
#include <cstring>

namespace n3 {
  
  typedef unsigned int UInt32;
  typedef unsigned short UInt16;
  typedef unsigned char UInt8;

  typedef UInt32 Label;
  typedef float Float;
  const Float FLOAT_MAX = FLT_MAX;

  typedef std::vector<float> fvec;
  typedef std::list<float> flist;
  typedef std::pair<int, int> IntPair;
  typedef std::pair<Label, Label> LabelPair;
  typedef std::pair<int, Label> IntLabelPair;

  inline void perr (const char* msg)
  {
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
  }

  inline void pwar (const char* msg) {std::cerr << msg << std::endl;}

  const double PI = 3.14159265359;
  const double LN2 = log(2.0);
  const double DUMMY = -1.0;
  const double EPSILON = 1e-16;
  const Label BGVAL = 0; /* Background point label */
  const int CRCONN = 8;	/* Contour region connectivity */
  const int CRCONN3 = 26; /* For a contour point where is a region point? */

};


#endif
