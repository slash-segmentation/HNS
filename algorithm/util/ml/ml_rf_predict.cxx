#include "ml_rf.h"

void classForest (int *mdim, int *ntest, int *nclass, int *maxcat,
		  int *nrnodes, int *ntree, double *x, double *xbestsplit,
		  double *pid, double *cutoff, double *countts, 
		  int *treemap, int *nodestatus, int *cat, int *nodeclass, 
		  int *jts, int *jet, int *bestvar, int *node, 
		  int *treeSize, int *keepPred, int *prox, double *proxMat, 
		  int *nodes);

void predict (void** argout, int nargout[], void** argin, int N, int D)
{
  int p_size = D;
  int mdim = p_size;
  int n_size = N;
  int nsample = n_size;
  int dimx[] = {p_size, n_size};
  int nclass = getScalar<int>(argin[11]);
  int* cat = (int*)getData(argin[15]);
  int maxcat = getScalar<int>(argin[16]);
  int sampsize = n_size;
  int nsum = sampsize;
  int strata = 1;
  int addclass = 0;
  int importance = 0;
  int localImp = 0;
  int proximity = getScalar<int>(argin[13]);
  int oob_prox = 0;
  int do_trace = 1;
  int keep_forest = 1;
  int replace = 1;
  int stratify = 0;
  int keep_inbag = 0;
  int* options = createNumericMatrix(10, 1, 0);
  options[0] = addclass;
  options[1] = importance;
  options[2] = localImp;
  options[3] = proximity;
  options[4] = oob_prox;
  options[5] = do_trace;
  options[6] = keep_forest;
  options[7] = replace;
  options[8] = stratify;
  options[9] = keep_inbag;
  int ntree = getScalar<int>(argin[2]);
  int nt = ntree;
  int ipi = 0;
  double* cutoff = (double*)getData(argin[5]);
  int nodesize = 1;
  double prox = 1.0;
  double impout = (double)p_size;
  double impSD = 1.0;
  double impmat = 1.0;
  double* X = getPr(argin[0]);
  int nrnodes = getScalar<int>(argin[1]);
  int* ndbigtree = (int*)getData(argin[10]);
  int* nodestatus = (int*)getData(argin[7]);
  int* bestvar = (int*)getData(argin[9]);
  int* treemap = (int*)getData(argin[6]);
  int* nodeclass = (int*)getData(argin[8]);
  double* xbestsplit = (double*)getData(argin[3]);
  int testdat = 0;
  double xts = 1.0;
  int clts = 1;
  int ntest = n_size;
  double* countts = NULL;
  int outclts = 1;
  int labelts = 0;
  double proxts = 1.0;
  double errts = 1.0;
  int* jts = NULL;
  int* jet = NULL;
  int keepPred = getScalar<int>(argin[12]);
  int nodes = getScalar<int>(argin[14]);
  int* nodexts = NULL;
  // int ndim = 2;
  int dims_ntest[] = {1, 1};
  if (nodes) {
    dims_ntest[0] = ntest;
    dims_ntest[1] = ntree;
    argout[4] = createNumericMatrix<int>(dims_ntest[0], dims_ntest[1], 0);
    nargout[8] = dims_ntest[0];
    nargout[9] = dims_ntest[1];
    nodexts = (int*)getData(argout[4]);
  }
  else {
    dims_ntest[0] = ntest;
    dims_ntest[1] = 1;
    argout[4] = createNumericMatrix<int>(dims_ntest[0], dims_ntest[1], 0);
    nargout[8] = dims_ntest[0];
    nargout[9] = dims_ntest[1];
    nodexts = (int*)getData(argout[4]);
  }
  double* proxMat = NULL;
  if (proximity) {
    dims_ntest[0] = ntest;
    dims_ntest[1] = ntest;
    argout[3] = createNumericMatrix<double>(dims_ntest[0], 
					    dims_ntest[1], 0.0);
    nargout[6] = dims_ntest[0];
    nargout[7] = dims_ntest[1];
    proxMat = (double*)getData(argout[3]);
  }
  else {
    dims_ntest[0] = 1;
    dims_ntest[1] = 1;
    argout[3] = createNumericMatrix<double>(dims_ntest[0], 
					    dims_ntest[1], 0.0);
    nargout[6] = dims_ntest[0];
    nargout[7] = dims_ntest[1];
    proxMat = (double*)getData(argout[3]);
    proxMat[0] = 1;
  }
  int* treeSize = ndbigtree;
  double* pid = (double*)getData(argin[4]);
  dims_ntest[0] = ntest;
  dims_ntest[1] = 1;
  argout[0] = createNumericMatrix<int>(dims_ntest[0], dims_ntest[1], 0);
  nargout[0] = dims_ntest[0];
  nargout[1] = dims_ntest[1];
  dims_ntest[0] = nclass;
  dims_ntest[1] = ntest;
  argout[2] = createNumericMatrix<double>(dims_ntest[0], dims_ntest[1], 0.0);
  nargout[4] = dims_ntest[0];
  nargout[5] = dims_ntest[1];
  jet = (int*)getData(argout[0]);
  if (keepPred) {
    dims_ntest[0] = ntest;
    dims_ntest[1] = ntree;
    argout[1] = createNumericMatrix<int>(dims_ntest[0], dims_ntest[1], 0);
    nargout[2] = dims_ntest[0];
    nargout[3] = dims_ntest[1];
    jts = (int*)getData(argout[1]);
  }
  else {
    dims_ntest[0] = ntest;
    dims_ntest[1] = 1;
    argout[1] = createNumericMatrix<int>(dims_ntest[0], dims_ntest[1], 0);
    nargout[2] = dims_ntest[0];
    nargout[3] = dims_ntest[1];
    jts = (int*)getData(argout[1]);
  }
  countts = getPr(argout[2]);
  classForest(&mdim, &ntest, &nclass, &maxcat, &nrnodes, &ntree, X, 
	      xbestsplit, pid, cutoff, countts, treemap, nodestatus, cat, 
	      nodeclass, jts, jet, bestvar, nodexts, treeSize, &keepPred, 
	      &proximity, proxMat, &nodes);
  del(&options);
}



void predict (int*& Y, double*& votes, int*& prediction_per_tree, 
	      double*& proximity_ts, int*& nodes, double* X, 
	      int N, int D, Model& model, 
	      PredictExtraOptions& extra_options)
{
  int opt_predict_all = 0;
  int opt_proximity = 0;
  int opt_nodes = 0;
  if (extra_options.predict_all > 0) {
    opt_predict_all = extra_options.predict_all;
  }
  if (extra_options.proximity > 0) opt_proximity = extra_options.proximity;
  if (extra_options.nodes > 0) opt_nodes = extra_options.nodes;
  int* ncat = NULL;
  int n_ncat = 0;
  if (model.categorical_feature != NULL) {
    for (int i = 0; i < D; i++) {
      if (model.categorical_feature[i]) {
	for (int j = 0; j < model.n_orig_uniques_in_feature[i]; j++) {
	  for (int k = 0; k < N; k++) {
	    if (X[k * D + i] == model.mapped_uniques_in_feature[i][j]) {
	      X[k * D + i] = model.mapped_uniques_in_feature[i][j];
	    }
	  }
	}
      }
    }
    ncat = copyNumericVector(model.ncat, model.ncat[0] * model.ncat[1]);
    n_ncat = model.ncat[0] * model.ncat[1];
  }
  else {
    ncat = createNumericMatrix<int>(1, D, 1);
    n_ncat = D;
  }
  int maxcat = find_max(ncat, n_ncat);
  void** argout = new void*[5];
  int nargout[10];
  void** argin = new void*[17];
  argin[0] = (void*)X;
  argin[1] = (void*)createIntScalar(model.nrnodes);
  argin[2] = (void*)createIntScalar(model.ntree);
  argin[3] = (void*)transpose(model.xbestsplit, model.n_xbestsplit[0], 
			      model.n_xbestsplit[1]);
  argin[4] = (void*)transpose(model.classwt, model.n_classwt[0], 
			      model.n_classwt[1]);
  argin[5] = (void*)transpose(model.cutoff, model.n_cutoff[0], 
			      model.n_cutoff[1]);
  argin[6] = (void*)transpose(model.treemap, model.n_treemap[0], 
			      model.n_treemap[1]);
  argin[7] = (void*)transpose(model.nodestatus, model.n_nodestatus[0], 
			      model.n_nodestatus[1]);
  argin[8] = (void*)transpose(model.nodeclass, model.n_nodeclass[0], 
			      model.n_nodeclass[1]);
  argin[9] = (void*)transpose(model.bestvar, model.n_bestvar[0], 
			      model.n_bestvar[1]);
  argin[10] = (void*)transpose(model.ndbigtree, model.n_ndbigtree[0], 
			       model.n_ndbigtree[1]);
  argin[11] = (void*)createIntScalar(model.nclass);
  argin[12] = (void*)&opt_predict_all;
  argin[13] = (void*)&opt_proximity;
  argin[14] = (void*)&opt_nodes;
  argin[15] = (void*)ncat; // It is a vector, so need to tranpose
  argin[16] = (void*)&maxcat;
  predict(argout, nargout, argin, N, D);
  Y = copyNumericVector((int*)argout[0], nargout[0] * nargout[1]);
  votes = copyNumericVector((double*)argout[2], nargout[4] * nargout[5]);
  prediction_per_tree = copyNumericVector((int*)argout[1], 
					  nargout[2] * nargout[3]);
  proximity_ts = copyNumericVector((double*)argout[3], 
				   nargout[6] * nargout[7]);
  nodes = copyNumericVector((int*)argout[4], nargout[8] * nargout[9]);
  int n = model.n_new_labels[0] * model.n_new_labels[1];
  for (int i = 1; i < N; i++) {    
    for (int j = 0; j < n; j++) {
      if (Y[i] == model.new_labels[j]) {
	Y[i] = model.orig_labels[j];
      }
    }
  }
  del(&ncat);
  del((int**)&argout[0]);
  del((int**)&argout[1]);
  del((double**)&argout[2]);
  del((double**)&argout[3]);
  del((int**)&argout[4]);
  del(&argout);
  del((int**)&argin[1]);
  del((int**)&argin[2]);
  del((double**)&argin[3]);
  del((double**)&argin[4]);
  del((double**)&argin[5]);
  del((int**)&argin[6]);
  del((int**)&argin[7]);
  del((int**)&argin[8]);
  del((int**)&argin[9]);
  del((int**)&argin[10]);
  del((int**)&argin[11]);
  del(&argin);
}
