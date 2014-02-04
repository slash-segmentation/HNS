#ifndef _n3_point_h_
#define _n3_point_h_

#include "n3_base.h"

namespace n3 {

  /* Point */
  class Point 
  {

  public:
    float x, y;

    Point (float x = -1.0, float y = -1.0): x(x), y(y) {}

    Point (Point const& p): x(p.x), y(p.y) {}

    float get_distance (Point const& p) const {
      double dx = x - p.x, dy = y - p.y;
      return (float)sqrt(dx * dx + dy * dy);
    }

    float get_angle (Point const& p) const {
      return atan2(p.y - y, p.x - x);
    }

    float get_curvature (Point const& ppre, Point const& pnext) const {
      double dx = ppre.x + pnext.x - 2.0 * x;
      double dy = ppre.y + pnext.y - 2.0 * y;
      return (float)(dx * dx + dy * dy);
    }

    float get_curve_angle (Point const& ppre, Point const& pnext) const {
      double vx10 = x - ppre.x, vy10 = y - ppre.y;
      double vx21 = pnext.x - x, vy21 = pnext.y - y;
      double den = sqrt(vx10 * vx10 + vy10 * vy10) * 
	sqrt(vx21 * vx21 + vy21 * vy21);
      double num = vx10 * -vx21 + vy10 * -vy21;
      double r = num /den;
      if (r > 1.0) r = 1.0;
      else if (r < -1.0) r = -1.0;
      return acos(r);
    }
    
    bool is_neighbor (Point const& p) const {
      return floor(get_distance(p) + 0.5) <= 1.0;
    }

    bool operator == (Point const& p) const {
      return fabs(x - p.x) < EPSILON && fabs(y - p.y) < EPSILON;}
    
    bool operator != (Point const& p) const {
      return fabs(x - p.x) >= EPSILON || fabs(y - p.y) >= EPSILON;}
    
    bool operator < (Point const& p) const {
      return x < p.x || (x == p.x && y < p.y);
    }

    Point operator + (Point const& p) const {
      return Point(x + p.x, y + p.y);
    }

    Point operator - (Point const& p) const {
      return Point(x - p.x, y - p.y);
    }

    double operator * (Point const& p) const {
      return (double)x * (double)p.x + (double)y * (double)p.y;
    }

    double operator * (fvec const& p) const {
      return (double)x * (double)p[0] + (double)y * (double)p[1];
    }

    Point operator / (double r) const {
      return Point(x / r, y / r);
    } 

    friend std::ostream& operator << (std::ostream& os, Point const& p) {
      os << p.x << " " << p.y;
      return os;
    }

    friend std::istream& operator >> (std::istream& is, Point& p) {
      is >> p.x >> p.y;
      return is;
    }

    void print (std::ostream& os) const {
      os << "(" << x << ", " << y << ")";
    }

  };



  /* Bounding box */
  class Box {

  public:
    Point start;
    float width, height;

    Box (Point start = Point(-1.0, -1.0), float width = 0.0, 
	 float height = 0.0)
      : start(start), width(width), height(height) {}

    static Box getUnion (Box const& b0, Box const& b1) {
      Box ret;
      ret.start.x = b0.start.x < b1.start.x? b0.start.x: b1.start.x;
      ret.start.y = b0.start.y < b1.start.y? b0.start.y: b1.start.y;
      /* Not subtract/add 1 for all */
      float xend0(b0.start.x + b0.width), yend0(b0.start.y + b0.height), 
	xend1(b1.start.x + b1.width), yend1(b1.start.y + b1.height);
      float xend = xend0 > xend1? xend0: xend1, 
	yend = yend0 > yend1? yend0: yend1;
      ret.width = xend - ret.start.x;
      ret.height = yend - ret.start.y;
      return ret;
    }

    friend std::ostream& operator << (std::ostream& os, Box const& b) {
      os << b.start << " " << b.width << " " << b.height;
      return os;
    }
    
    friend std::istream& operator >> (std::istream& is, Box& b) {
      is >> b.start >> b.width >> b.height;
      return is;
    }
    
  };



  /* Rotated bounding box */
  class RotBox : public Box {
    
  public:
    Point center;
    float angle;

    RotBox (Point center = Point(-1.0, -1.0), float width = 0.0, 
	    float height = 0.0, float angle = 0.0) 
      : Box(Point(-1.0, -1.0), width, height), angle(angle) {}

    friend std::ostream& operator << (std::ostream& os, RotBox const& b) {
      os << b.center << " " << b.width << " " 
	 << b.height << " " << b.angle;
      return os;
    }

    friend std::istream& operator >> (std::istream& is, RotBox& b) {
      is >> b.center >> b.width >> b.height >> b.angle;
      return is;
    }

  };


  
  /* Points */
  class Points {

  public: 
    typedef std::list<Point> PointList;
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

    void push_back (Point const& p) {body.push_back(p);}

    void push_front (Point const& p) {body.push_front(p);}

    void pop_back () {body.pop_back();}

    void pop_front () {body.pop_front();}

    Point& front () {return body.front();}

    Point const& front () const {return body.front();}

    Point& back () {return body.back();}

    Point const& back () const {return body.back();}

    iterator insert (iterator pos, Point const& p) {
      return body.insert(pos, p);
    }

    iterator erase (iterator it) {return body.erase(it);}

    iterator erase (iterator first, iterator last) {
      return body.erase(first, last);
    }

    void splice (iterator pos, Points& x) {body.splice(pos, x.get_body());}

    void splice (iterator pos, Points& x, iterator i) {
      body.splice(pos, x.get_body(), i);
    }

    void splice (iterator pos, Points& x, iterator first, iterator last) {
      body.splice(pos, x.get_body(), first, last);
    }

    void unique () {body.unique();}

    void merge (Points& x) {body.merge(x.get_body());}

    void sort () {body.sort();}

    void reverse () {body.reverse();}

    friend std::ostream& operator << (std::ostream& os, Points const& p) {
      os << p.size() << " ";
      for (const_iterator pit = p.begin(); pit != p.end(); ++pit) 
	os << *pit << " ";
      return os;
    }

    friend std::istream& operator >> (std::istream& is, Points& p) {
      int n;
      is >> n;
      for (int i = 0; i < n; ++i) {
	Point pt;
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
    Point get_centroid () const;

    /* Assume all points are in order */
    double get_length (bool closed = false) const;

    /* Assume all points are in order */
    void get_curvatures (flist& c, bool closed = false) const;

    /* Assume all points are in order */
    void get_curve_angles (flist& a, bool closed) const;

    /* Get bounding box */
    Box get_box () const;

    /* Assume i is not end() */
    iterator next (Points::iterator i, bool loop) {
      iterator ret = i;
      ++ret;
      if (loop && ret == body.end()) ret = body.begin();
      return ret;
    }

    /* Assume i is not end() */
    const_iterator next (Points::const_iterator i, bool loop) const {
      const_iterator ret = i;
      ++ret;
      if (loop && ret == body.end()) ret = body.begin();
      return ret;
    }

    iterator previous (Points::iterator i, bool loop) {
      iterator ret = i;
      if (ret == body.begin()) {
	ret = body.end();
	if (loop) --ret;
      }
      else --ret;
      return ret;
    }

    const_iterator previous (Points::const_iterator i, bool loop) const {
      const_iterator ret = i;
      if (ret == body.begin()) {
	ret = body.end();
	if (loop) --ret;
      }
      else --ret;
      return ret;
    }

    reverse_iterator turn (iterator it) {
      int n = -1;
      for (iterator i = it; i != body.end(); ++i) ++n;
      reverse_iterator ret = body.rbegin();
      std::advance(ret, n);
      return ret;
    }

    const_reverse_iterator turn (const_iterator it) const {
      int n = -1;
      for (const_iterator i = it; i != body.end(); ++it) ++n;
      const_reverse_iterator ret = body.rbegin();
      std::advance(ret, n);
      return ret;
    }

    iterator turn (reverse_iterator it) {
      int n = -1;
      for (reverse_iterator i = it; i != body.rend(); ++i) ++n;
      iterator ret = body.begin();
      std::advance(ret, n);
      return ret;
    }

    const_iterator turn (const_reverse_iterator it) const {
      int n = -1;
      for (const_reverse_iterator i = it; i != body.rend(); ++i) ++n;
      const_iterator ret = body.begin();
      std::advance(ret, n);
      return ret;
    }
    

  };



  /* Pixel */
  template <typename T> class Pixel : public virtual Point 
  {

  public: 
    T val;
  

    Pixel (float x = -1.0, float y = -1.0, T val = 0): 
    Point (x, y), val(val) {}


    Pixel (Point const& p, T val): Point(p), val(val) {}


    Point get_point () const {return Point(x, y);}


    bool operator < (Pixel<T> const& p) const {return val < p.val;}


    friend std::ostream& operator << (std::ostream& os, 
				      Pixel<T> const& p) {
      os << p.x << " " << p.y << " " << p.val;
      return os;
    }

    
    friend std::istream& operator >> (std::istream& is, Pixel<T>& p) {
      is >> p.x >> p.y >> p.val;
      return is;
    }

  };


  typedef Pixel<Float> fPixel;


  template <typename T> double 
    getLength (T const& p, bool closed) {
    double ret = 0.0;
    for (typename T::const_iterator it = p.begin(); it != p.end(); ++it)
      ret += it->get_length(closed);
    return ret;
  }


  template <typename T> void 
    getCurvatures (flist& c, T const& p, bool closed) {
    for (typename T::const_iterator it = p.begin(); it != p.end(); ++it) {
      flist cv;
      it->get_curvatures(cv, closed);
      c.splice(c.end(), cv);
    }
  }

};

#endif
