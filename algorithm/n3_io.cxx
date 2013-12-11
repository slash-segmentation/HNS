#include "n3_io.h"
using namespace n3;

/* Get file row number */
int n3::getr (const char* file)
{
  int ret = -1;
  std::ifstream fs(file);
  if (fs.is_open()) {
    ret = 0;
    std::string s;
    while (fs.good()) {
      if (!getline(fs, s)) break;
      ++ret;
    }
    fs.close();
  }
  else perr("Error: cannot read file...");
  return ret;
}



/* Get file column number */
int n3::getc (const char* file)
{
  int ret = -1;
  std::ifstream fs(file);
  if (fs.is_open()) {
    ret = 0;
    std::string s;
    if (getline(fs, s)) {
      std::stringstream ss(s);
      std::string t;
      while (ss >> t) ++ret;
    }
    fs.close();
  }
  else perr("Error: cannot read file...");
  return ret;
}
