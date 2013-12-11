cmake_minimum_required(VERSION 2.6)

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

add_executable(genTextonDict 
  gadget/main_genTextonDict.cxx
  util/struct_util.cxx 
  util/cv/cv_texton.cxx
  util/cv/cv_textond.cxx
  util/ml/ml_kmeans.cxx 
  struct/point.cxx)
target_link_libraries(genTextonDict 
  ${ITK_LIBRARIES})

add_executable(watershed 
  gadget/main_watershed.cxx)
target_link_libraries(watershed 
  ${ITK_LIBRARIES})

add_executable(merge 
  tree2d/main_merge.cxx 
  tree2d/tree2d_util.cxx 
  util/struct_util.cxx 
  struct/point.cxx)
target_link_libraries(merge 
  ${ITK_LIBRARIES})

add_executable(genMerges
  tree2d/main_genMerges.cxx
  tree2d/tree2d_util.cxx
  util/struct_util.cxx 
  struct/point.cxx)
target_link_libraries(genMerges 
  ${ITK_LIBRARIES})

add_executable(genBoundaryFeatures
  tree2d/main_genBoundaryFeatures.cxx
  tree2d/bc.cxx
  tree2d/tree2d_util.cxx
  util/struct_util.cxx 
  util/cv/cv_hist.cxx
  util/cv/cv_texton.cxx
  struct/point.cxx)
target_link_libraries(genBoundaryFeatures 
  ${ITK_LIBRARIES})

add_executable(genBoundaryLabels
  tree2d/main_genBoundaryLabels.cxx
  tree2d/tree2d_util.cxx
  util/metric/metric_pairf.cxx
  util/metric/metric_util.cxx
  util/struct_util.cxx 
  struct/point.cxx)
target_link_libraries(genBoundaryLabels
  gmp gmpxx
  ${ITK_LIBRARIES})

add_executable(segment 
  tree2d/main_segment.cxx 
  tree2d/tree2d_util.cxx
  util/struct_util.cxx
  struct/point.cxx)
target_link_libraries(segment 
  ${ITK_LIBRARIES})

add_executable(genOrderedContours 
  gadget/main_genOrderedContours.cxx 
  util/struct_util.cxx 
  util/cv/cv_contour.cxx
  struct/point.cxx)
target_link_libraries(genOrderedContours
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(genRegionPairs 
  link3d/main_genRegionPairs.cxx 
  util/struct_util.cxx
  struct/point.cxx)
target_link_libraries(genRegionPairs 
  ${ITK_LIBRARIES})

add_executable(genRegionStats 
  link3d/main_genRegionStats.cxx 
  link3d/link3d_io.cxx
  link3d/rs.cxx
  util/io_util.cxx
  util/struct_util.cxx 
  util/cv/cv_contour.cxx
  util/cv/cv_curve.cxx
  struct/point.cxx)
target_link_libraries(genRegionStats 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(genSectionFeatures 
  link3d/main_genSectionFeatures.cxx 
  link3d/link3d_io.cxx
  link3d/sc.cxx
  util/cv/cv_contour.cxx
  util/io_util.cxx
  util/struct_util.cxx 
  struct/point.cxx)
target_link_libraries(genSectionFeatures 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(genSectionLabels 
  link3d/main_genSectionLabels.cxx 
  link3d/link3d_io.cxx
  util/io_util.cxx
  util/struct_util.cxx
  struct/point.cxx)
target_link_libraries(genSectionLabels 
  ${ITK_LIBRARIES})

add_executable(threshold 
  link3d/main_threshold.cxx 
  link3d/link3d_io.cxx 
  util/io_util.cxx 
  util/struct_util.cxx 
  struct/point.cxx)
target_link_libraries(threshold 
  ${ITK_LIBRARIES})