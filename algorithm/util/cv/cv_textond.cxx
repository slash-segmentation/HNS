#include "cv_texton.h"
#include "util/ml/ml_kmeans.h"
using namespace n3;

void n3::genTextonDict (TextonDict& dict, 
			std::list<TextonPatch> const& pats, 
			int maxIterationNum, double convergenceThreshold, 
			int patchLength)
{
  int n = pats.size();
  int m = pats.front().size();
  float** samples = new float*[n];
  int i = 0;
  for (std::list<TextonPatch>::const_iterator it = pats.begin(); 
       it != pats.end(); ++it) {
    samples[i] = new float[m];
    for (int j = 0; j < m; ++j) samples[i][j] = (*it)[j];
    ++i;
  }
  int* membership = new int[n];
  float** clusters = 
    omp_kmeans(0, samples, m, n, TEXTON_BIN, 
	       convergenceThreshold, membership, maxIterationNum);
  dict.resize(TEXTON_BIN);
  for (int c = 0; c < TEXTON_BIN; ++c) {
    dict[c].resize(patchLength);
    for (int j = 0; j < m; ++j) dict[c][j] = clusters[c][j];
  }
  delete[] membership;
  for (i = 0; i < n; ++i) delete[] samples[i];
  delete[] samples;
}
