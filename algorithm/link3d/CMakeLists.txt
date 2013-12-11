cmake_minimum_required(VERSION 2.6)

project(link3d)

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

include_directories(..)

add_executable(genRegionPairs 
  main_genRegionPairs.cxx 
  ../util/struct_util.cxx
  ../struct/point.cxx)
target_link_libraries(genRegionPairs 
  ${ITK_LIBRARIES})

add_executable(genRegionStats 
  main_genRegionStats.cxx 
  link3d_io.cxx
  rs.cxx
  ../util/io_util.cxx
  ../util/struct_util.cxx 
  ../util/cv/cv_contour.cxx
  ../util/cv/cv_curve.cxx
  ../struct/point.cxx)
target_link_libraries(genRegionStats 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(genSectionFeatures 
  main_genSectionFeatures.cxx 
  link3d_io.cxx
  sc.cxx
  ../util/cv/cv_contour.cxx
  ../util/io_util.cxx
  ../util/struct_util.cxx 
  ../struct/point.cxx)
target_link_libraries(genSectionFeatures 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(genSectionLabels 
  main_genSectionLabels.cxx 
  link3d_io.cxx
  ../util/io_util.cxx
  ../util/struct_util.cxx
  ../struct/point.cxx)
target_link_libraries(genSectionLabels 
  ${ITK_LIBRARIES})

add_executable(threshold 
  main_threshold.cxx 
  link3d_io.cxx 
  ../util/io_util.cxx 
  ../util/struct_util.cxx 
  ../struct/point.cxx)
target_link_libraries(threshold 
  ${ITK_LIBRARIES})

add_executable(link 
  main_link.cxx 
  ../util/io_util.cxx 
  ../util/struct_util.cxx 
  ../struct/point.cxx)
target_link_libraries(link 
  ${ITK_LIBRARIES})

add_executable(genBodyStats 
  main_genBodyStats.cxx 
  bs.cxx
  link3d_io.cxx
  ../util/cv/cv_curve3.cxx
  ../util/cv/cv_curve.cxx
  ../util/cv/cv_contour.cxx
  ../util/io_util.cxx
  ../util/struct_util.cxx 
  ../struct/point.cxx 
  ../struct/point3.cxx)
target_link_libraries(genBodyStats 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(genBodyPairs 
  main_genBodyPairs.cxx 
  link3d_io.cxx
  ../util/io_util.cxx 
  ../util/struct_util.cxx
  ../struct/point.cxx)
target_link_libraries(genBodyPairs 
  ${ITK_LIBRARIES})

add_executable(genBodyPairLabels 
  main_genBodyPairLabels.cxx 
  link3d_io.cxx
  ../util/io_util.cxx
  ../util/struct_util.cxx
  ../struct/point.cxx)
target_link_libraries(genBodyPairLabels 
  ${ITK_LIBRARIES})

add_executable(genBodyPairFeatures 
  main_genBodyPairFeatures.cxx 
  link3d_io.cxx
  bs.cxx
  sc.cxx  
  bc.cxx
  ../util/cv/cv_contour.cxx
  ../util/cv/cv_curve.cxx
  ../util/cv/cv_curve3.cxx
  ../util/io_util.cxx
  ../util/struct_util.cxx 
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(genBodyPairFeatures 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(threshold2
  main_threshold2.cxx 
  link3d_io.cxx 
  ../util/io_util.cxx 
  ../util/struct_util.cxx 
  ../struct/point.cxx)
target_link_libraries(threshold2 
  ${ITK_LIBRARIES})