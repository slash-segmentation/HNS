#include "point3.h"
using namespace n3;

// Get centroid of all points
Point3 n3::Points3::get_centroid () const 
{
  int n = body.size();
  if (n == 0) perr("Error: cannot get centroid of no points...");
  double x = 0.0, y = 0.0, z = 0.0;
  for (std::list<Point3>::const_iterator it = body.begin(); 
       it != body.end(); ++it) {
    x += it->x;
    y += it->y;
    z += it->z;
  }
  x /= (double)n;
  y /= (double)n;
  z /= (double)n;
  return Point3(x, y, z);
}
