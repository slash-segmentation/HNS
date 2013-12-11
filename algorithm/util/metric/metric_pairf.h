#ifndef _n3_metric_pairf_h_
#define _n3_metric_pairf_h_

#include "metric_util.h"

namespace n3 {

  /* Count in same way as ISBI Challenge 2012 */
  void getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
		      std::map<LabelPair, BigInt> const& overlaps);

  /* Compute precision */
  BigFloat getPrecision (BigInt const& tp, BigInt const& fp, 
			 int prec = DEFAULT_BIG_FLOAT_PREC);

  /* Compute recall */
  BigFloat getRecall (BigInt const& tp, BigInt const& fn, 
		      int prec = DEFAULT_BIG_FLOAT_PREC);

  /* Compute F-score with class scores */
  /* Recommended for precision safety */
  BigFloat getF (BigInt const& tp, BigInt const& fp, BigInt const& fn, 
		 int prec);

  /* Compute F-score with precision and recall */
  /* Not recommended for precision safety */
  BigFloat getF (BigFloat const& precision, BigFloat const& recall);

  /* Given a reference image and a set of regions compute scores */
  void getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
		      std::list<const Points*> const& segs, 
		      const Points* bg, LabelImage::Pointer refImage);

  void getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
		      std::list<const Points3*> const& segs, 
		      const Points3* bg, LabelImage3::Pointer refImage);

  /* Given two images compute scores */
  void getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
		      LabelImage::Pointer resImage, 
		      LabelImage::Pointer refImage);

  void getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
		      LabelImage3::Pointer resImage, 
		      LabelImage3::Pointer refImage);

  /* Given two 3D volumes compute total 3D scores */
  void getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
		      std::vector<LabelImage::Pointer> const& resVolume, 
		      std::vector<LabelImage::Pointer> const& refVolume);

  void getPairScores (BigInt& tp, BigInt& tn, BigInt& fp, BigInt& fn, 
		      std::vector<LabelImage3::Pointer> const& resVolume, 
		      std::vector<LabelImage3::Pointer> const& refVolume);

  /* Given a reference image and a set of regions */
  /* Compute pair precision/recall/F-score */
  std::vector<BigFloat> 
    getPairScores (std::list<const Points*> const& segs, 
		   const Points* bg, LabelImage::Pointer refImage, 
		   int prec = DEFAULT_BIG_FLOAT_PREC);

  std::vector<BigFloat> 
    getPairScores (std::list<const Points3*> const& segs, 
		   const Points3* bg, LabelImage3::Pointer refImage, 
		   int prec = DEFAULT_BIG_FLOAT_PREC);

};

#endif
