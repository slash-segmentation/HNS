cmake_minimum_required(VERSION 2.6)

project(misc)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
# find_package(OpenCV REQUIRED)

set(GMP_INCLUDE_DIR "" CACHE FILEPATH "GMP include directory")
set(GMP_LIB_DIR "" CACHE FILEPATH "GMP library directory")
include_directories(${GMP_INCLUDE_DIR})
link_directories(${GMP_LIB_DIR})


include_directories(..)
# include_directories(../3rdparty)


add_executable(mergeByOverlap
  main_mergeByOverlap.cxx 
  ../util/struct_util.cxx 
  ../struct/point.cxx)
target_link_libraries(mergeByOverlap 
  ${ITK_LIBRARIES})

add_executable(mergeByCentroid
  main_mergeByCentroid.cxx 
  ../util/struct_util.cxx 
  ../struct/point.cxx)
target_link_libraries(mergeByCentroid 
  ${ITK_LIBRARIES})

add_executable(mergeByLink
  main_mergeByLink.cxx 
  ../link3d/link3d_io.cxx
  ../util/struct_util.cxx 
  ../util/io_util.cxx
  ../struct/point.cxx)
target_link_libraries(mergeByLink 
  ${ITK_LIBRARIES})