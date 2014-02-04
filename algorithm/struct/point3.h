#ifndef _n3_point3_h_
#define _n3_point3_h_

#include "point.h"

namespace n3 {

  class Point3 : public virtual Point
  {

  public:
    float z;

    Point3 (float x = -1.0, float y = -1.0, float z = -1.0) 
      : Point(x, y), z(z) {}

    Point3 (Point const& p, float z) : Point(p), z(z) {}

    Point3 (Point3 const& p) : Point(p.x, p.y), z(p.z) {}

    float get_distance (Point3 const& p) const {
      double dx = x - p.x, dy = y - p.y, dz = z - p.z;
      return (float)sqrt(dx * dx + dy * dy + dz * dz);
    }

    float get_curve_angle (Point3 const& ppre, Point3 const& pnext) const {
      double vx10 = x - ppre.x, vy10 = y - ppre.y, vz10 = z - ppre.z;
      double vx21 = pnext.x - x, vy21 = pnext.y - y, vz21 = pnext.z - z;
      double den = sqrt(vx10 * vx10 + vy10 * vy10 + vz10 * vz10) * 
	sqrt(vx21 * vx21 + vy21 * vy21 + vz21 * vz21);
      double num = vx10 * -vx21 + vy10 * -vy21 + vz10 * -vz21;
      double r = num / den;
      if (r > 1.0) r = 1.0;
      else if (r < -1.0) r = -1.0;
      return acos(r);
    }

    bool operator == (Point3 const& p) const {
      return (fabs(x - p.x) < EPSILON && fabs(y - p.y) < EPSILON 
	      && fabs(z - p.z) < EPSILON);
    }

    Point3 operator + (Point3 const& p) const {
      return Point3(x + p.x, y + p.y, z + p.z);
    }

    Point3 operator - (Point3 const& p) {
      return Point3(x - p.x, y - p.y, z - p.z);
    }

    Point3 operator / (double r) const {
      return Point3(x / r, y / r, z / r);
    } 

    bool operator < (Point3 const& p) const {
      return x < p.x || (x == p.x && (y < p.y || y == p.y && z < p.z));
    }

    double operator * (fvec const& p) const {
      return ((double)x * (double)p[0] + (double)y * (double)p[1] 
	      + (double)z * p[2]);
    }

    friend std::ostream& operator << (std::ostream& os, Point3 const& p) {
      os << p.x << " " << p.y << " " << p.z;
      return os;
    }

    friend std::istream& operator >> (std::istream& is, Point3& p) {
      is >> p.x >> p.y >> p.z;
      return is;
    }

    void print (std::ostream& os) const {
      os << "(" << x << ", " << y << ", " << z << ")";
    }

  };



  class Box3 : public Box 
  {
    
  public:
    Point3 start;
    float deep;

    Box3 (Point3 start = Point3(), float width = -1.0, float height = -1.0, 
	  float deep = -1.0) 
      : Box(start, width, height), start(start), deep(deep) {}

    static Box3 getUnion (Box3 const& b0, Box3 const& b1) {
      Box3 ret;
      ret.start.x = b0.start.x < b1.start.x? b0.start.x: b1.start.x;
      ret.start.y = b0.start.y < b1.start.y? b0.start.y: b1.start.y;
      ret.start.z = b0.start.z < b1.start.z? b0.start.z: b1.start.z;
      /* Not subtract/add 1 for all */
      float xend0(b0.start.x + b0.width), yend0(b0.start.y + b0.height), 
	zend0(b0.start.z + b0.deep), xend1(b1.start.x + b1.width), 
	yend1(b1.start.y + b1.height), zend1(b1.start.z + b1.deep);
      float xend = xend0 > xend1? xend0: xend1, 
	yend = yend0 > yend1? yend0: yend1, 
	zend = zend0 > zend1? zend0: zend1;
      ret.width = xend - ret.start.x;
      ret.height = yend - ret.start.y;
      ret.deep = zend - ret.start.z;
      return ret;
    }

    friend std::ostream& operator << (std::ostream& os, Box3 const& b) {
      os << b.start << " " << b.width << " " << b.height << " " 
	 << b.deep;
      return os;
    }
    
    friend std::istream& operator >> (std::istream& is, Box3& b) {
      is >> b.start >> b.width >> b.height >> b.deep;
      return is;
    }

  };



  class Points3 : public Points 
  {
  public: 
    typedef std::list<Point3> PointList;
    typedef PointList::iterator iterator;
    typedef PointList::const_iterator const_iterator;
    typedef PointList::reverse_iterator reverse_iterator;
    typedef PointList::const_reverse_iterator const_reverse_iterator;    
    
  private: 
    PointList body;

  public:
    PointList& get_body () {return body;}

    PointList const& get_body () const {return body;}

    iterator begin () {return body.begin();}
    
    const_iterator begin() const {return body.begin();}

    iterator end () {return body.end();}

    const_iterator end () const {return body.end();}

    reverse_iterator rbegin () {return body.rbegin();}

    const_reverse_iterator rbegin () const {return body.rbegin();}

    reverse_iterator rend () {return body.rend();}

    const_reverse_iterator rend () const {return body.rend();}

    iterator last () {return --body.end();}

    const_iterator last () const {return --body.end();}

    unsigned int size () const {return body.size();}

    bool empty () const {return body.empty();}

    void clear () {body.clear();}

    void push_back (Point3 const& p) {body.push_back(p);}

    void push_front (Point3 const& p) {body.push_front(p);}

    void pop_back () {body.pop_back();}

    void pop_front () {body.pop_front();}

    Point3& front () {return body.front();}

    Point3 const& front () const {return body.front();}

    Point3& back () {return body.back();}

    Point3 const& back () const {return body.back();}

    iterator insert (iterator pos, Point3 const& p) {
      return body.insert(pos, p);
    }
    
    iterator erase (iterator it) {return body.erase(it);}

    iterator erase (iterator first, iterator last) {
      return body.erase(first, last);
    }

    void splice (iterator pos, Points3& x) {body.splice(pos, x.get_body());}

    void splice (iterator pos, Points3& x, iterator i) {
      body.splice(pos, x.get_body(), i);
    }

    void splice (iterator pos, Points3& x, iterator first, iterator last) {
      body.splice(pos, x.get_body(), first, last);
    }

    void unique () {body.unique();}

    void merge (Points3& x) {body.merge(x.get_body());}

    void sort () {body.sort();}

    void reverse () {body.reverse();}

    friend std::ostream& operator << (std::ostream& os, Points3 const& p) {
      os << p.size() << " ";
      for (const_iterator pit = p.begin(); pit != p.end(); ++pit) 
	os << *pit << " ";
      return os;
    }

    friend std::istream& operator >> (std::istream& is, Points3& p) {
      int n;
      is >> n;
      for (int i = 0; i < n; ++i) {
	Point3 pt;
	is >> pt;
	p.push_back(pt);
      }
      return is;
    }

    void print (std::ostream& os) const {
      for (const_iterator it = begin(); it != end(); ++it) {
	it->print(os);
	os << " ";
      }
    }

    /* Get centroid of all points */
    Point3 get_centroid () const;

    // Assume all points are in order
    void get_curve_angles (flist& a, bool closed) const;

    /* Get bounding box */
    Box3 get_box () const;

  };



  template <typename T> class Pixel3 : public Point3, public Pixel<T> 
    {
      
    public:
      Pixel3 (float x = -1.0, float y = -1.0, float z = -1.0, T val = 0) :
      Point(x, y), Point3(x, y, z), Pixel<T>(x, y, val) {}

      Pixel3 (Point3 const& p, T val) : 
      Point(p.x, p.y), Point3(p.x, p.y, p.z), Pixel<T>(p.x, p.y, val) {}

      Point3 get_point () const {return Point3(x, y, z);}

      bool operator < (Pixel3<T> const& p) {return Pixel<T>::val < p.val;}

      friend std::ostream& operator << (std::ostream& os, 
					Pixel3<T> const& p) {
	os << p.x << " " << p.y << " " << p.z << " " << p.val;
	return os;
      }

      friend std::istream& operator >> (std::istream& is, Pixel3<T>& p) {
	is >> p.x >> p.y >> p.z >> p.val;
	return is;
      }
      
    };

  typedef Pixel3<Float> fPixel3;
  typedef Pixel3<Label> lPixel3;

};

#endif
