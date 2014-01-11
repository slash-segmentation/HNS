cmake_minimum_required(VERSION 2.6)

project(tree3d)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(OpenCV REQUIRED)

set(GMP_INCLUDE_DIR "" CACHE FILEPATH "GMP include directory")
set(GMP_LIB_DIR "" CACHE FILEPATH "GMP library directory")
include_directories(${GMP_INCLUDE_DIR})
link_directories(${GMP_LIB_DIR})


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