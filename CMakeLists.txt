cmake_minimum_required (VERSION 2.6)
project(unc)

include_directories(src gen)
add_library(unc src/unc/unc.cpp)
add_library(uncdata gen/unc/iso8859.cpp gen/unc/unccase.cpp)

set_target_properties(unc PROPERTIES COMPILE_FLAGS "-fPIC")
set_target_properties(uncdata PROPERTIES COMPILE_FLAGS "-fPIC")
