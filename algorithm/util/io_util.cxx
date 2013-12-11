#include "io_util.h"
using namespace n3;

// If writen == true first write n
void n3::writePoints (std::ofstream& fs, Points const& points, 
		      bool writen, char delim0, char delim1)
{
  if (writen) fs << points.size() << delim1;
  for (Points::const_iterator it = points.begin(); it != points.end(); 
       ++it) {
    writePoint(fs, *it, delim0);
    fs << delim1;
  }
}



// If readn == true first read in n and then read in n points
// Otherwise if n < 0 read in as many points as possible
// Otherwise read in n points
bool n3::readPoints (Points& points, std::istringstream& is, bool readn, 
		     int n, char delim)
{
  bool ret;
  if (readn) if(!(ret = read(n, is, (int)0, delim))) return false;
  if (n == 0) return true;
  int cnt = 0;
  Point p(-1.0, -1.0);
  while (readPoint(p, is, delim)) {
    points.push_back(p);
    ++cnt;
    if (n > 0 && cnt >= n) break;
  }
  if (cnt == n || n < 0) return true;
}
