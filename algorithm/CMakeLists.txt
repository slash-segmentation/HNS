cmake_minimum_required(VERSION 2.8)

project(neuro3d)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
find_package(OpenCV REQUIRED)

find_package(OpenMP)
if (OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif(OPENMP_FOUND)

set(GMP_INCLUDE_DIR "" CACHE FILEPATH "GMP include directory")
set(GMP_LIB_DIR "" CACHE FILEPATH "GMP library directory")
include_directories(${GMP_INCLUDE_DIR})
link_directories(${GMP_LIB_DIR})

include_directories(.)

add_executable(hnsGenTextonDict 
  gadget/main_genTextonDict.cxx
  util/struct_util.cxx 
  util/cv/cv_texton.cxx
  util/cv/cv_textond.cxx
  util/ml/ml_kmeans.cxx 
  struct/point.cxx)
target_link_libraries(hnsGenTextonDict 
  ${ITK_LIBRARIES})

add_executable(hnsWatershed 
  gadget/main_watershed.cxx)
target_link_libraries(hnsWatershed 
  ${ITK_LIBRARIES})

add_executable(hnsMerge 
  tree2d/main_merge.cxx 
  tree2d/tree2d_util.cxx 
  util/struct_util.cxx 
  struct/point.cxx)
target_link_libraries(hnsMerge 
  ${ITK_LIBRARIES})

add_executable(hnsGenMerges
  tree2d/main_genMerges.cxx
  tree2d/tree2d_util.cxx
  util/struct_util.cxx 
  struct/point.cxx)
target_link_libraries(hnsGenMerges 
  ${ITK_LIBRARIES})

add_executable(hnsGenBoundaryFeatures
  tree2d/main_genBoundaryFeatures.cxx
  tree2d/bc.cxx
  tree2d/tree2d_util.cxx
  util/struct_util.cxx 
  util/cv/cv_hist.cxx
  util/cv/cv_texton.cxx
  struct/point.cxx)
target_link_libraries(hnsGenBoundaryFeatures 
  ${ITK_LIBRARIES})

add_executable(hnsGenBoundaryLabels
  tree2d/main_genBoundaryLabels.cxx
  tree2d/tree2d_util.cxx
  util/metric/metric_pairf.cxx
  util/metric/metric_util.cxx
  util/struct_util.cxx 
  struct/point.cxx)
target_link_libraries(hnsGenBoundaryLabels
  gmp gmpxx
  ${ITK_LIBRARIES})

add_executable(hnsSegment 
  tree2d/main_segment.cxx 
  tree2d/tree2d_util.cxx
  util/struct_util.cxx
  struct/point.cxx)
target_link_libraries(hnsSegment 
  ${ITK_LIBRARIES})

add_executable(hnsGenOrderedContours 
  gadget/main_genOrderedContours.cxx 
  util/struct_util.cxx 
  util/cv/cv_contour.cxx
  struct/point.cxx)
target_link_libraries(hnsGenOrderedContours
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(hnsGenRegionPairs 
  link3d/main_genRegionPairs.cxx 
  util/struct_util.cxx
  struct/point.cxx)
target_link_libraries(hnsGenRegionPairs 
  ${ITK_LIBRARIES})

add_executable(hnsGenRegionStats 
  link3d/main_genRegionStats.cxx 
  link3d/link3d_io.cxx
  link3d/rs.cxx
  util/io_util.cxx
  util/struct_util.cxx 
  util/cv/cv_contour.cxx
  util/cv/cv_curve.cxx
  util/cv/cv_texton.cxx
  struct/point.cxx)
target_link_libraries(hnsGenRegionStats 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(hnsGenSectionFeatures 
  link3d/main_genSectionFeatures.cxx 
  link3d/link3d_io.cxx
  link3d/sc.cxx
  util/cv/cv_contour.cxx
  util/cv/cv_hist.cxx
  util/io_util.cxx
  util/struct_util.cxx 
  struct/point.cxx)
target_link_libraries(hnsGenSectionFeatures 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(hnsGenSectionLabels 
  link3d/main_genSectionLabels.cxx 
  link3d/link3d_io.cxx
  util/io_util.cxx
  util/struct_util.cxx
  struct/point.cxx)
target_link_libraries(hnsGenSectionLabels 
  ${ITK_LIBRARIES})

add_executable(hnsThreshold 
  link3d/main_threshold.cxx 
  link3d/link3d_io.cxx 
  util/io_util.cxx 
  util/struct_util.cxx 
  struct/point.cxx)
target_link_libraries(hnsThreshold 
  ${ITK_LIBRARIES})

add_executable(hnsChooseWatershedThreshold 
  tree2d/main_chooseWatershedThreshold.cxx 
  tree2d/tree2d_util.cxx 
  util/struct_util.cxx
  util/metric/metric_util.cxx 
  util/metric/metric_pairf.cxx 
  struct/point.cxx)
target_link_libraries(hnsChooseWatershedThreshold 
  gmp gmpxx 
  ${ITK_LIBRARIES})