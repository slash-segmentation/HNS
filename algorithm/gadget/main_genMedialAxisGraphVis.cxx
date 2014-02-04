#include "util/cv/cv_ma.h"
using namespace n3;

void operation (const char* magNodeFileName, const char* magEdgeFileName, 
		const char* magFileName)
{
  MaGraph mag(0);
  readMedialAxisGraph(mag, magFileName);
  if (magNodeFileName != NULL) {
    std::ofstream fn(magNodeFileName);
    if (fn.is_open()) {
      mag.print(fn, "node");
      fn.close();
    }
    else perr("Error: cannot create medial axis graph vis node file...");
  }
  if (magEdgeFileName != NULL) {
    std::ofstream fe(magEdgeFileName);
    if (fe.is_open()) {
      mag.print(fe, "edge");
      fe.close();
    }
    else perr("Error: cannot create medial axis graph vis edge file...");
  }
}



int main (int argc, char* argv[])
{
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] 
	      << " medialAxisGraphFileName"
	      << " medialAxisGraphVisNodeFileName ('NULL' to skip)"
	      << " medialAxisGraphVisEdgeFileName ('NULL' to skip)"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* magFileName = argv[argi++];
  const char* magNodeFileName = 
    strcmp(argv[argi], "NULL") == 0? NULL: argv[argi];
  ++argi;
  const char* magEdgeFileName = 
    strcmp(argv[argi], "NULL") == 0? NULL: argv[argi];
  ++argi;
  operation(magNodeFileName, magEdgeFileName, magFileName);
  return EXIT_SUCCESS;
}
