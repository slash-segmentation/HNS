#include "point.h"
using namespace n3;

// Get centroid of all points
Point n3::Points::get_centroid () const 
{
  int n = body.size();
  if (n == 0) perr("Error: cannot get centroid of no points...");
  double x = 0.0, y = 0.0;
  for (std::list<Point>::const_iterator it = body.begin(); 
       it != body.end(); ++it) {
    x += it->x;
    y += it->y;    
  }
  x /= (double)n;
  y /= (double)n;
  return Point(x, y);
}



// Assume all points are in order
double n3::Points::get_length (bool closed) const
{
  double ret = 0.0;
  const_iterator it0 = begin();
  const_iterator it1 = it0; ++it1;
  while (it1 != end()) {
    ret += it0->get_distance(*it1);
    ++it0;
    ++it1;
  }
  if (closed) ret += it0->get_distance(*begin());
  return ret;
}



// Assume all points are in order
void n3::Points::get_curvatures (flist& c, bool closed) const
{
  if (size() < 3) return;
  const_iterator it0 = begin();
  const_iterator it1 = it0; ++it1;
  const_iterator it2 = it1; ++it2;
  while (it2 != end()) {
    c.push_back(it1->get_curvature(*it0, *it2));
    ++it0;
    ++it1;
    ++it2;
  }
  if (closed) {
    it2 = begin();
    c.push_back(it1->get_curvature(*it0, *it2));
    ++it0;
    it1 = begin();
    ++it2;
    c.push_back(it1->get_curvature(*it0, *it2));
  }
}



// Assume all points are in order
void n3::Points::get_curve_angles (flist& a, bool closed) const
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



/* Get bounding box */
Box n3::Points::get_box () const
{
  float xmin = body.front().x, ymin = body.front().y;
  float xmax = xmin, ymax = ymin;
  for (const_iterator it = body.begin(); it != body.end(); ++it) {
    if (it->x < xmin) xmin = it->x;
    else if (it->x > xmax) xmax = it->x;
    if (it->y < ymin) ymin = it->y;
    else if (it->y > ymax) ymax = it->y;
  }
  return Box(Point(xmin, ymin), xmax - xmin + 1.0, ymax - ymin + 1.0);
}



// /* Splice p to end of pout */
// void n3::splice (Points& pout, Points& p, bool unique)
// {
//   pout.splice(pout.end(), p);
//   if (unique) {
//     pout.sort();
//     pout.unique();
//   }
// }



// /* Append p to pout */
// void n3::append (Points& pout, Points const& p, bool unique)
// {
//   Points t = p;
//   pout.splice(pout.end(), t);
//   if (unique) {
//     pout.sort();
//     pout.unique();
//   }
// }



// /* Combine p0 and p1 to pout */
// void n3::combine (Points& pout, Points const& p0, Points const& p1, 
// 		  bool unique) 
// {
//   pout = p0;
//   append(pout, p1, unique);
// }



// /* Remove pr from p */
// void n3::remove (Points& p, Points const& pr)
// {
//   for (Points::iterator pit = p.begin(); pit != p.end(); ++pit) {
//     for (Points::const_iterator rit = pr.begin(); 
// 	 rit != pr.end(); ++rit) {
//       if (*pit == *rit) {
// 	pit = p.erase(pit);
// 	--pit;
// 	break;
//       }
//     }
//   }
// }
