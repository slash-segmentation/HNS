cmake_minimum_required(VERSION 2.6)

project(tree3d)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(OpenCV REQUIRED)

set(GMP_INCLUDE_DIR "" CACHE FILEPATH "GMP include directory")
set(GMP_LIB_DIR "" CACHE FILEPATH "GMP library directory")
include_directories(${GMP_INCLUDE_DIR})
link_directories(${GMP_LIB_DIR})

enable_language(Fortran)
set(CMAKE_Fortran_FLAGS "-fpic -m64")


include_directories(..)


add_executable(merge 
  main_merge.cxx 
  tree3d_util.cxx
  ../tree2d/tree2d_util.cxx 
  ../util/struct_util.cxx 
  ../util/struct_util3.cxx
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(merge 
  ${ITK_LIBRARIES})

add_executable(genMerges
  main_genMerges.cxx
  tree3d_util.cxx
  ../tree2d/tree2d_util.cxx
  ../util/struct_util.cxx 
  ../util/struct_util3.cxx
  ../struct/point.cxx 
  ../struct/point3.cxx)
target_link_libraries(genMerges 
  ${ITK_LIBRARIES})

add_executable(genBoundaryFeatures
  main_genBoundaryFeatures.cxx
  bc.cxx
  tree3d_util.cxx
  ../tree2d/bc.cxx
  ../tree2d/tree2d_util.cxx
  ../util/struct_util.cxx 
  ../util/struct_util3.cxx
  ../util/cv/cv_hist.cxx
  ../util/cv/cv_texton.cxx
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(genBoundaryFeatures 
  ${ITK_LIBRARIES})

add_executable(genBoundaryLabels
  main_genBoundaryLabels.cxx
  tree3d_util.cxx
  ../tree2d/tree2d_util.cxx
  ../util/metric/metric_pairf.cxx
  ../util/metric/metric_util.cxx
  ../util/struct_util.cxx 
  ../util/struct_util3.cxx
  ../struct/point.cxx 
  ../struct/point3.cxx)
target_link_libraries(genBoundaryLabels
  gmp gmpxx
  ${ITK_LIBRARIES})

add_executable(segment 
  main_segment.cxx 
  tree3d_util.cxx
  ../tree2d/tree2d_util.cxx
  ../util/struct_util.cxx
  ../util/struct_util3.cxx
  ../struct/point.cxx 
  ../struct/point3.cxx)
target_link_libraries(segment 
  ${ITK_LIBRARIES})

add_executable(genRegionFeatures
  main_genRegionFeatures.cxx
  rc.cxx
  tree3d_util.cxx
  ../tree2d/tree2d_util.cxx
  ../util/struct_util.cxx 
  ../util/struct_util3.cxx
  ../util/cv/cv_ma.cxx
  ../util/cv/cv_curve.cxx
  ../util/cv/cv_curve3.cxx
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(genRegionFeatures 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(genRegionLabels 
  main_genRegionLabels.cxx 
  tree3d_util.cxx 
  ../util/struct_util.cxx
  ../util/struct_util3.cxx 
  ../struct/point.cxx 
  ../struct/point3.cxx)
target_link_libraries(genRegionLabels 
  ${ITK_LIBRARIES})

add_executable(postSegment 
  main_postSegment.cxx 
  tree3d_util.cxx
  ../tree2d/tree2d_util.cxx
  ../util/struct_util.cxx
  ../util/struct_util3.cxx
  ../struct/point.cxx 
  ../struct/point3.cxx)
target_link_libraries(postSegment 
  ${ITK_LIBRARIES})

add_executable(genMedialAxisGraph 
  main_genMedialAxisGraph.cxx 
  ../util/cv/cv_ma.cxx
  ../util/cv/cv_curve.cxx
  ../util/cv/cv_curve3.cxx
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(genMedialAxisGraph 
  ${ITK_LIBRARIES}
  ${OpenCV_LIBS})

add_executable(genRegionFeaturesMAG
  main_genRegionFeaturesMAG.cxx
  rc.cxx
  tree3d_util.cxx
  ../tree2d/tree2d_util.cxx
  ../util/struct_util.cxx 
  ../util/struct_util3.cxx
  ../util/cv/cv_ma.cxx
  ../util/cv/cv_curve.cxx
  ../util/cv/cv_curve3.cxx
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(genRegionFeaturesMAG 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(itrGenMerges 
  main_itrGenMerges.cxx 
  bc.cxx 
  tree3d_util.cxx
  tree3d_ml.cxx
  ../tree2d/bc.cxx
  ../tree2d/tree2d_util.cxx
  ../util/struct_util.cxx
  ../util/struct_util3.cxx
  ../util/cv/cv_hist.cxx
  ../util/cv/cv_texton.cxx
  ../util/metric/metric_pairf.cxx
  ../util/metric/metric_util.cxx
  ../util/ml/ml_rf_predict.cxx
  ../util/ml/ml_rf_model.cxx
  ../util/ml/ml_rf_util.cxx
  ../util/ml/rf_util/classRF.cpp
  ../util/ml/rf_util/classTree.cpp
  ../util/ml/rf_util/cokus.cpp
  ../util/ml/rf_util/qsort.c
  ../util/ml/rf_util/rfsub.f
  ../util/ml/rf_util/rfutils.cpp
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(itrGenMerges
  gmp gmpxx
  ${ITK_LIBRARIES})

add_executable(genStochasticMerges
  main_genStochasticMerges.cxx
  tree3d_util.cxx
  perturb.cxx
  ../tree2d/tree2d_util.cxx
  ../util/struct_util.cxx
  ../util/struct_util3.cxx
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(genStochasticMerges 
  ${ITK_LIBRARIES})

add_executable(multiTreeSegment 
  main_multiTreeSegment.cxx 
  tree3d_util.cxx
  ../tree2d/tree2d_util.cxx
  ../util/struct_util.cxx
  ../util/struct_util3.cxx
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(multiTreeSegment 
  ${ITK_LIBRARIES})

add_executable(matchSegment
  main_matchSegment.cxx 
  tree3d_util.cxx
  ../util/struct_util.cxx
  ../util/struct_util3.cxx
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(matchSegment 
  ${ITK_LIBRARIES})