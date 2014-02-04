#ifndef _n3_io_h_
#define _n3_io_h_

#include "n3_base.h"

namespace n3 {

  inline bool exist (const char* file)
  {
    std::ifstream fs(file);
    return fs;
  }



  /* Get file row number */
  int getr (const char* file);
  /* Get file column number */
  int getc (const char* file);


  template <typename TVal> inline void 
    write (std::ostream& os, TVal const& data) 
    {
      /* os << (fabs((double)data) > EPSILON? data: 0); */
      os << data;
    }



  /* Write npr items per row, separated with delim */
  /* If npr < 0 then write all data in one row */
  template <typename TContainer> void 
    write (std::ostream& os, TContainer const& data, 
	   int npr, char delim = ' ')
    {
      if (npr == 0) return;
      if (npr < 0) npr = data.size();
      int c = 1;
      for (typename TContainer::const_iterator it = data.begin(); 
	   it != data.end(); ++it) {
	write(os, *it);
	os << (c++ % npr == 0? '\n': delim);
      }
    }


  /* Write npr items per row, separated with delim */
  /* If npr < 0 then write all data in one row */
  template <typename TContainer> void 
    write (const char* file, TContainer const& data, int npr, 
	   char delim = ' ') 
    {
      std::ofstream fs(file);
      if (fs.is_open()) {
	write(fs, data, npr, delim);
	fs.close();
      }
      else perr("Error: cannot create file...");
    }


  /* Data is a container of containers */
  template <typename TContainer> void 
    write2 (std::ostream& os, TContainer const& data, 
	    bool writen = false, char delim0 = ' ', char delim1 = '\n')
    {
      if (writen) os << data.size() << delim0;
      for (typename TContainer::const_iterator it0 = data.begin(); 
	   it0 != data.end(); ++it0) {
	if (writen) os << it0->size() << delim0;
	typename TContainer::value_type::const_iterator it1 = it0->begin();
	while (it1 != it0->end()) {
	  write(os, *it1);
	  ++it1;
	  if (it1 == it0->end()) {
	    os << delim1;
	    break;
	  }
	  else os << delim0;
	}
      }
    }



  /* Data is a container of containers */
  template <typename TContainer> void 
    write2 (const char* file, TContainer const& data, 
	    bool writen = false, char delim0 = ' ', char delim1 = '\n')
    {
      std::ofstream fs(file);
      if (fs.is_open()) {
	write2(fs, data, writen, delim0, delim1);
	fs.close();
      }
      else perr("Error: cannot create file...");
    }



  /* Assume read is successful */
  template <typename TVal> TVal 
    read (std::istringstream& ss, TVal null, char delim = ' ')
    {
      std::string s;
      getline(ss, s, delim);
      return (TVal)atof(s.c_str());
    }



  /* Return false if read is unsuccessful */
  template <typename TVal> inline bool 
    read (TVal& v, std::istringstream& is, TVal null, char delim = ' ')
    {
      std::string s;
      if (!getline(is, s, delim)) return false;
      v = (TVal)atof(s.c_str());
      return true;
    }



  /* Read npr items per row, separated with delim */
  /* If npr < 0 then read all data in each row */
  template <typename TContainer> bool 
    read (TContainer& data, std::istream& is, 
	  int npr, char delim = ' ')
    {
      if (npr == 0) return true;
      while (is.good()) {
	std::string l, d;
	if (!getline(is, l)) break;
	std::istringstream ss(l);
	if (ss.good()) {
	  if (npr < 0) {
	    typename TContainer::value_type v;
	    while (read(v, ss, (typename TContainer::value_type)0, 
			delim)) data.insert(data.end(), v);
	  }
	  else {
	    int cnt = 0;	  
	    for (int i = 0; i < npr; ++i) {	    
	      data.insert(data.end(), 
			  read(ss, (typename TContainer::value_type)0, 
			       delim));
	      ++cnt;
	    }
	    if (cnt != npr) return false;
	  }
	}
      }
      return true;
    }



  template <typename TContainer> void
    read (TContainer& data, const char* file, int npr, char delim = ' ')
    {
      std::ifstream fs(file);
      if (fs.is_open()) {
	if(!read(data, fs, npr, delim)) 
	  perr("Error: data file may be corrupted...");	  
	fs.close();
      }
      else perr("Error: cannot read file...");
    }



  template <typename TVal> void
    read2 (std::vector<std::vector<TVal> >& data, std::istream& is)
    {
      int n = -1;
      is >> n;
      data.reserve(n);
      for (int i = 0; i < n; ++i) {
	int m = -1;
	is >> m;
	data.insert(data.end(), std::vector<TVal>(m));
	for (int j = 0; j < m; ++j) is >> data[i][j];
      }
    }


  /* Read in a container of containers */
  /* Each row is an element of outer container */
  /* Need to know row # and col # */
  template <typename TContainer> void 
    read2 (TContainer& data, std::istream& is, int nrow, int ncol) 
    {
      for (int i = 0; i < nrow; ++i) {
	data.insert(data.end(), typename TContainer::value_type());
	for (int j = 0; j < ncol; ++j) {
	  typename TContainer::value_type::value_type v;
	  is >> v;
	  data.back().insert(data.back().end(), v);
	}
      }
    }


  /* Read in a container of containers */
  /* Each row is an element of outer container */
  template <typename TContainer> void 
    read2 (TContainer& data, const char* file) {
    std::ifstream fs(file);
    if (fs.is_open()) {
      read2(data, fs, getr(file), getc(file));
      fs.close();
    }
    else perr("Error: cannot read file...");
  }
  
};

#endif
