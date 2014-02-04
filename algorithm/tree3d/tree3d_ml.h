#ifndef _n3_tree3d_ml_h_
#define _n3_tree3d_ml_h_
#include "util/ml/ml_rf.h"
#include "util/metric/metric_pairf.h"
#include "tree3d_util.h"

namespace n3 {

  namespace tree3d {

    /* (l0, l1) -> (feats, saliency) */
    /* Saliency is probability of splitting */
    typedef std::map<LabelPair, std::pair<flist, double> > fBoundaryTable3;

    // Saliency = merge probability
    // Since we use priority queue (top is always 'largest')
    // We put merge probability as merge value
    // But output 'saliency' is actually split probability (1 - saliency)
    void getBoundaryTable (fBoundaryTable3& bt, PointLabelMap3 const& lmap, 
			   PointMap3 const& rmap, PointMap3 const& cmap, 
			   PointMap3 const& bmap, 
			   LabelImage3::Pointer canvas, 
			   FloatImage3::Pointer rawImage, 
			   FloatImage3::Pointer pbImage, 
			   rf::Model const& rfModel);

    void getBoundaryTable (fBoundaryTable3& bt, tree2d::MergeQueue& mq, 
			   PointLabelMap3 const& lmap, 
			   PointMap3 const& rmap, PointMap3 const& cmap, 
			   PointMap3 const& bmap, 
			   LabelImage3::Pointer canvas, 
			   FloatImage3::Pointer rawImage, 
			   FloatImage3::Pointer pbImage, 
			   rf::Model const& rfModel);

    // Collect features if pFeats != NULL
    // Collect labels if pTruthImage != NULL && pLabels != NULL
    // Use mq = NULL to skip updating it
    // Use removed = NULL to skip collecting removed label pairs
    void updateBoundaryTable (std::list<flist>* pFeats, 
			      std::list<int>* pLabels, 
			      fBoundaryTable3& bt, tree2d::MergeQueue* mq, 
			      std::set<LabelPair>* removed, 
			      PointMap3& rmap, PointMap3& cmap, 
			      PointLabelMap3& lmap, PointMap3& bmap, 
			      Label l0, Label l1, Label l01, 
			      LabelImage3::Pointer canvas, 
			      FloatImage3::Pointer rawImage, 
			      FloatImage3::Pointer pbImage, 
			      rf::Model const& rfModel, 
			      LabelImage3::Pointer* pTruthImage);

    // Generate merges
    // Collect features if pFeats != NULL
    // Collect labels if pTruthImage != NULL && pLabels != NULL
    void getMerges (std::list<tree2d::fMerge>& merges, 
		    std::list<flist>* pFeats, 
		    std::list<int>* pLabels, PointMap3& rmap, 
		    PointMap3& cmap, PointLabelMap3& lmap, 
		    PointMap3& bmap, LabelImage3::Pointer canvas, 
		    FloatImage3::Pointer rawImage, 
		    FloatImage3::Pointer pbImage, 
		    rf::Model const& rfModel, 
		    LabelImage3::Pointer* pTruthImage);

  };

};

#endif
