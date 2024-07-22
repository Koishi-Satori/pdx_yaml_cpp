cmake_minimum_required(VERSION 3.21)
project(pdx_yaml_native)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(pdx_yaml_native SHARED pdx_yaml.cpp)
add_executable(pdx_yaml_test test.cpp)
add_library(pdx_yaml_py SHARED pdx_yaml_py.cpp)
target_link_libraries(pdx_yaml_test pdx_yaml_native)
target_link_libraries(pdx_yaml_py pdx_yaml_native)
