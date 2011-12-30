cmake_minimum_required (VERSION 2.6)
project(unc)

include_directories(src gen)
add_library(unc src/unc.cpp)
add_library(uncdata gen/iso8859.cpp gen/unccase.cpp)
