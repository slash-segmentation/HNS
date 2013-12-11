cmake_minimum_required(VERSION 2.6)

project(ml)

find_package(OpenMP)

if (OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories(..)

enable_language(Fortran)
set(CMAKE_Fortran_FLAGS "-fpic -m64")

add_executable(rf_train 
  main_rf_train.cxx
  ../util/ml/ml_rf_train.cxx
  ../util/ml/ml_rf_model.cxx
  ../util/ml/ml_rf_util.cxx
  ../util/ml/rf_util/classRF.cpp
  ../util/ml/rf_util/classTree.cpp
  ../util/ml/rf_util/cokus.cpp
  ../util/ml/rf_util/qsort.c
  ../util/ml/rf_util/rfsub.f
  ../util/ml/rf_util/rfutils.cpp)

add_executable(rf_predict
  main_rf_predict.cxx
  ../util/ml/ml_rf_predict.cxx
  ../util/ml/ml_rf_model.cxx
  ../util/ml/ml_rf_util.cxx
  ../util/ml/rf_util/classRF.cpp
  ../util/ml/rf_util/classTree.cpp
  ../util/ml/rf_util/cokus.cpp
  ../util/ml/rf_util/qsort.c
  ../util/ml/rf_util/rfsub.f
  ../util/ml/rf_util/rfutils.cpp)

add_executable(rf_checkImportance 
  main_rf_checkImportance.cxx
  ../util/ml/ml_rf_model.cxx)