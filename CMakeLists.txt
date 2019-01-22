cmake_minimum_required(VERSION 3.10)

project(MXCore)

set (MXSchedule_VERSION_MAJOR 0)
set (MXSchedule_VERSION_MINOR 1)

set (CMAKE_CXX_STANDARD 11)

file (GLOB_RECURSE SOURCE "Src/*.cpp" "Src/*.c")
file (GLOB_RECURSE HEADER "Inc/*.h" "Interface/*.h")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Interface)

add_library(MXSchedule STATIC ${SOURCE})
