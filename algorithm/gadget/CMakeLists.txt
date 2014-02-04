cmake_minimum_required(VERSION 2.6)

project(gadget)

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

add_executable(genTextonDict 
  main_genTextonDict.cxx
  ../util/struct_util.cxx 
  ../util/cv/cv_texton.cxx
  ../util/cv/cv_textond.cxx
  ../util/ml/ml_kmeans.cxx 
  ../struct/point.cxx)
target_link_libraries(genTextonDict 
  ${ITK_LIBRARIES})

add_executable(genTextonDict3 
  main_genTextonDict3.cxx
  ../util/struct_util.cxx 
  ../util/struct_util3.cxx
  ../util/cv/cv_texton.cxx
  ../util/cv/cv_textond.cxx
  ../util/ml/ml_kmeans.cxx 
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(genTextonDict3
  ${ITK_LIBRARIES})

add_executable(eval 
  main_eval.cxx
  ../util/struct_util.cxx 
  ../util/metric/metric_pairf.cxx
  ../util/metric/metric_util.cxx
  ../struct/point.cxx)
target_link_libraries(eval
  gmp gmpxx
  ${ITK_LIBRARIES})

add_executable(vi 
  main_vi.cxx)
target_link_libraries(vi
  ${ITK_LIBRARIES})

add_executable(overlay 
  main_overlay.cxx 
  ../util/vis_util.cxx)
target_link_libraries(overlay 
  ${ITK_LIBRARIES})

add_executable(applyColormap 
  main_applyColormap.cxx)
target_link_libraries(applyColormap 
  ${ITK_LIBRARIES})

add_executable(relabelImage 
  main_relabelImage.cxx)
target_link_libraries(relabelImage 
  ${ITK_LIBRARIES})

add_executable(relabelImage3 
  main_relabelImage3.cxx)
target_link_libraries(relabelImage3 
  ${ITK_LIBRARIES})

add_executable(blurImage 
  main_blurImage.cxx)
target_link_libraries(blurImage 
  ${ITK_LIBRARIES})

add_executable(blurImage3 
  main_blurImage3.cxx)
target_link_libraries(blurImage3 
  ${ITK_LIBRARIES})

add_executable(invertImage 
  main_invertImage.cxx)
target_link_libraries(invertImage 
  ${ITK_LIBRARIES})

add_executable(dilateImage 
  main_dilateImage.cxx 
  ../util/struct_util.cxx
  ../util/cv/cv_contour.cxx 
  ../struct/point.cxx)
target_link_libraries(dilateImage
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(dilateImage2 
  main_dilateImage2.cxx 
  ../util/struct_util.cxx
  ../struct/point.cxx)
target_link_libraries(dilateImage2
  ${ITK_LIBRARIES})

add_executable(dilateImage3
  main_dilateImage3.cxx 
  ../util/struct_util3.cxx
  ../util/struct_util.cxx
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(dilateImage3
  ${ITK_LIBRARIES})

add_executable(saveAs 
  main_saveAs.cxx)
target_link_libraries(saveAs
  ${ITK_LIBRARIES})

add_executable(watershed 
  main_watershed.cxx)
target_link_libraries(watershed 
  ${ITK_LIBRARIES})

add_executable(watershed3 
  main_watershed3.cxx)
target_link_libraries(watershed3 
  ${ITK_LIBRARIES})

add_executable(i2f 
  main_i2f.cxx)
target_link_libraries(i2f 
  ${ITK_LIBRARIES})

add_executable(labelConnectedComponents 
  main_labelConnectedComponents)
target_link_libraries(labelConnectedComponents 
  ${ITK_LIBRARIES})

add_executable(thresholdImage 
  main_thresholdImage)
target_link_libraries(thresholdImage 
  ${ITK_LIBRARIES})

add_executable(thresholdImage3 
  main_thresholdImage3)
target_link_libraries(thresholdImage3 
  ${ITK_LIBRARIES})

add_executable(fillHoles 
  main_fillHoles)
target_link_libraries(fillHoles 
  ${ITK_LIBRARIES})

add_executable(concatPairs 
  main_concatPairs)

add_executable(matchSegToTruth
  main_matchSegToTruth.cxx 
  ../util/struct_util.cxx 
  ../util/metric/metric_util.cxx
  ../struct/point.cxx)
target_link_libraries(matchSegToTruth 
  gmp gmpxx
  ${ITK_LIBRARIES})

add_executable(genRegionPairLink
  main_genRegionPairLink.cxx
  ../util/struct_util.cxx
  ../struct/point.cxx)
target_link_libraries(genRegionPairLink 
  ${ITK_LIBRARIES})

add_executable(genOrderedContours 
  main_genOrderedContours.cxx 
  ../util/struct_util.cxx 
  ../util/cv/cv_contour.cxx
  ../struct/point.cxx)
target_link_libraries(genOrderedContours
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(testConnectivity
  main_testConnectivity.cxx 
  ../struct/point.cxx 
  ../struct/point3.cxx)
target_link_libraries(testConnectivity
  ${ITK_LIBRARIES})

add_executable(removeExclusiveGaps3 
  main_removeExclusiveGaps3.cxx)
target_link_libraries(removeExclusiveGaps3
  ${ITK_LIBRARIES})

add_executable(binaryThinning
  main_binaryThinning.cxx 
  ../util/cv/cv_ma.cxx
  ../util/cv/cv_curve.cxx
  ../util/cv/cv_curve3.cxx
  ../struct/point.cxx 
  ../struct/point3.cxx)
target_link_libraries(binaryThinning 
  ${ITK_LIBRARIES} 
  ${OpenCV_LIBS})

add_executable(genMedialAxisGraphVis 
  main_genMedialAxisGraphVis.cxx 
  ../util/cv/cv_ma.cxx
  ../util/cv/cv_curve.cxx
  ../util/cv/cv_curve3.cxx
  ../struct/point.cxx
  ../struct/point3.cxx)
target_link_libraries(genMedialAxisGraphVis
  ${ITK_LIBRARIES}
  ${OpenCV_LIBS})

add_executable(unique 
  main_unique.cxx 
  ../n3_io.cxx)