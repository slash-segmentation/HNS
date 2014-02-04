#include "tree2d_io.h"
#include "tree2d_util.h"
#include "tree_visit.h"
using namespace n3;
using namespace n3::tree2d;

void operation (const char* treeFileName0, const char* treeFileName1)
{
  std::list<fMerge> merges0, merges1;
  readMerges(merges0, treeFileName0, NULL);
  readMerges(merges1, treeFileName1, NULL);
  fTree tree0, tree1;
  getTree(tree0, merges0);
  getTree(tree1, merges1);
  std::list<Label> code0, code1;
  encode(code0, tree0);
  encode(code1, tree1);
  std::cout << (is_equal(code0, code1)? 1: 0) << std::endl;
}


int main (int argc, char* argv[])
{
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0]
	      << " treeFileName0"
	      << " treeFileName1"
	      << std::endl;
    return EXIT_FAILURE;
  }
  int argi = 1;
  const char* treeFileName0 = argv[argi++];
  const char* treeFileName1 = argv[argi++];
  operation(treeFileName0, treeFileName1);
  return EXIT_SUCCESS;
}
