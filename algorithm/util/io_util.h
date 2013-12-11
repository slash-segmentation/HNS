#ifndef _n3_io_util_h_
#define _n3_io_util_h_

#include "n3_io.h"
#include "struct/point.h"

namespace n3 {

  template <typename TContainer> void 
    writePairs (const char* file, TContainer const& pairs, 
		char delim0 = ' ', char delim1 = '\n')
    {
      std::ofstream fs(file);
      if (fs.is_open()) {
	for (typename TContainer::const_iterator it = pairs.begin(); 
	     it != pairs.end(); ++it) {
	  write(fs, it->first);
	  fs << delim0;
	  write(fs, it->second);
	  fs << delim1;
	} 
	fs.close();
      }
      else perr("Error: cannot create pair file...");
    }


  template <typename TContainer> void 
    readPairs (TContainer& pairs, const char* file, 
	       char delim0 = ' ', char delim1 = '\n')
    {
      std::ifstream fs(file);
      if (fs.is_open()) {
	std::string line;
	while (getline(fs, line)) {
	  std::istringstream is(line);
	  typename TContainer::value_type p;
	  while (read(p.first, is, (Label)0, delim0) && 
		 read(p.second, is, (Label)0, delim1)) 
	    pairs.insert(pairs.end(), p);
	}
	fs.close();
      }
      else perr("Error: cannot read pair file...");
    }


  inline void writePoint (std::ofstream& fs, Point const& p, 
			  char delim = ' ')
  {
    write(fs, p.x);
    fs << delim;
    write(fs, p.y);
  }


  inline bool readPoint (Point& p, std::istringstream& is, char delim = ' ')
  {
    return 
      read(p.x, is, (float)0, delim) && 
      read(p.y, is, (float)0, delim);
  }


  // If writen == true first write n
  void writePoints (std::ofstream& fs, Points const& points, 
		    bool writen, char delim0 = ' ', char delim1 = '\n');

  // If readn == true first read in n and then read in n points
  // Otherwise if n < 0 read in as many points as possible
  // Otherwise read in n points
  bool readPoints (Points& points, std::istringstream& is, bool readn, 
		   int n = -1, char delim = ' ');

};

#endif
