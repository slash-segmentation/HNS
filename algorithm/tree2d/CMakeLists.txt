cmake_minimum_required(VERSION 2.6)

project(tree2d)

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
  tree2d_util.cxx 
  ../util/struct_util.cxx 
  ../struct/point.cxx)
target_link_libraries(merge 
  ${ITK_LIBRARIES})

add_executable(genMerges
  main_genMerges.cxx
  tree2d_util.cxx
  ../util/struct_util.cxx 
  ../struct/point.cxx)
target_link_libraries(genMerges 
  ${ITK_LIBRARIES})

# add_executable(genFlatPbImage 
#   main_genFlatPbImage.cxx 
#   tree2d_util.cxx 
#   ../util/struct_util.cxx 
#   ../struct/point.cxx)
# target_link_libraries(genFlatPbImage 
#   ${ITK_LIBRARIES})

add_executable(genBoundaryFeatures
  main_genBoundaryFeatures.cxx
  bc.cxx
  tree2d_util.cxx
  ../util/struct_util.cxx 
  ../util/cv/cv_hist.cxx
  ../util/cv/cv_texton.cxx
  ../struct/point.cxx)
target_link_libraries(genBoundaryFeatures 
  ${ITK_LIBRARIES})

add_executable(genBoundaryLabels
  main_genBoundaryLabels.cxx
  tree2d_util.cxx
  ../util/metric/metric_pairf.cxx
  ../util/metric/metric_util.cxx
  ../util/struct_util.cxx 
  ../struct/point.cxx)
target_link_libraries(genBoundaryLabels
  gmp gmpxx
  ${ITK_LIBRARIES})

add_executable(segment 
  main_segment.cxx 
  tree2d_util.cxx
  ../util/struct_util.cxx
  ../struct/point.cxx)
target_link_libraries(segment 
  ${ITK_LIBRARIES})

add_executable(genPotPlotData 
  main_genPotPlotData.cxx 
  tree2d_util.cxx 
  ../util/metric/metric_util.cxx
  ../util/struct_util.cxx
  ../struct/point.cxx)
target_link_libraries(genPotPlotData 
  gmp gmpxx
  ${ITK_LIBRARIES})

add_executable(chooseWatershedThreshold 
  main_chooseWatershedThreshold.cxx 
  tree2d_util.cxx 
  ../util/struct_util.cxx
  ../util/metric/metric_util.cxx 
  ../util/metric/metric_pairf.cxx 
  ../struct/point.cxx)
target_link_libraries(chooseWatershedThreshold 
  gmp gmpxx 
  ${ITK_LIBRARIES})

add_executable(isTreeEqual
  main_isTreeEqual.cxx 
  tree2d_util.cxx
  ../util/struct_util.cxx
  ../struct/point.cxx)
target_link_libraries(isTreeEqual
  ${ITK_LIBRARIES})

# add_executable(segmentToContours 
#   main_segmentToContours.cxx 
#   tree2d_util.cxx
#   ../util/cv/cv_contour.cxx
#   ../util/struct_util.cxx
#   ../struct/point.cxx)
# target_link_libraries(segmentToContours 
#   ${ITK_LIBRARIES}
#   ${OpenCV_LIBS})

# add_executable(genRegionFeatures
#   main_genRegionFeatures.cxx
#   rc.cxx
#   ../util/struct_util.cxx 
#   ../util/cv/cv_contour.cxx
#   ../util/cv/cv_curve.cxx
#   ../struct/point.cxx)
# target_link_libraries(genRegionFeatures 
#   ${ITK_LIBRARIES}
#   ${OpenCV_LIBS})

# add_executable(genRegionLabels
#   main_genRegionLabels.cxx
#   tree2d_util.cxx
#   ../util/struct_util.cxx 
#   ../struct/point.cxx)
# target_link_libraries(genRegionLabels
#   ${ITK_LIBRARIES})

# add_executable(postSegment 
#   main_postSegment.cxx 
#   tree2d_util.cxx
#   ../util/struct_util.cxx
#   ../struct/point.cxx)
# target_link_libraries(postSegment 
#   ${ITK_LIBRARIES})

# add_executable(multiTreeSegment 
#   main_multiTreeSegment.cxx 
#   tree2d_util.cxx 
#   ../util/struct_util.cxx 
#   ../struct/point.cxx)
# target_link_libraries(multiTreeSegment 
#   ${ITK_LIBRARIES})

# add_executable(optSegment 
#   main_optSegment.cxx 
#   tree2d_util.cxx
#   ../util/struct_util.cxx
#   ../struct/point.cxx)
# target_link_libraries(optSegment 
#   ${ITK_LIBRARIES})

# add_executable(genStochasticMerges
#   main_genStochasticMerges.cxx
#   tree2d_util.cxx
#   perturb.cxx
#   ../util/struct_util.cxx 
#   ../struct/point.cxx)
# target_link_libraries(genStochasticMerges 
#   ${ITK_LIBRARIES})