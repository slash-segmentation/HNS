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



// Assume all points are in order
void n3::Points3::get_curve_angles (flist& a, bool closed) const
{
  if (size() < 3) return;
  const_iterator it0 = begin();
  const_iterator it1 = it0; ++it1;
  const_iterator it2 = it1; ++it2;
  while (it2 != end()) {
    a.push_back(it1->get_curve_angle(*it0, *it2));
    ++it0;
    ++it1;
    ++it2;
  }
  if (closed) {
    it2 = begin();
    a.push_back(it1->get_curve_angle(*it0, *it2));
    ++it0;
    it1 = begin();
    ++it2;
    a.push_front(it1->get_curve_angle(*it0, *it2));
  }
}



// Get bounding box
Box3 n3::Points3::get_box () const
{
  if (body.empty()) perr("Error: cannot get box of no points...");
  float xmin = body.front().x, xmax = body.front().x, 
    ymin = body.front().y, ymax = body.front().y, 
    zmin = body.front().z, zmax = body.front().z;
  for (const_iterator pit = body.begin(); pit != body.end(); ++pit) {
    if (pit->x < xmin) xmin = pit->x;
    if (pit->x > xmax) xmax = pit->x;
    if (pit->y < ymin) ymin = pit->y;
    if (pit->y > ymax) ymax = pit->y;
    if (pit->z < zmin) zmin = pit->z;
    if (pit->z > zmax) zmax = pit->z;
  }
  return Box3(Point3(xmin, ymin, zmin), xmax - xmin + 1, ymax - ymin + 1, 
	      zmax - zmin + 1);
}
