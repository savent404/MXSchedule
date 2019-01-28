cmake_minimum_required(VERSION 3.0)

project(MXCore)

option(TEST "enable test function.It depend on google-test" OFF)
set (MXSchedule_VERSION_MAJOR 0)
set (MXSchedule_VERSION_MINOR 1)

set (CMAKE_CXX_STANDARD 11)

file (GLOB_RECURSE SOURCE "Src/*.cpp" "Src/*.c")
file (GLOB_RECURSE HEADER "Inc/*.h" "Interface/*.h")


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Interface)

add_library(MXSchedule STATIC ${SOURCE})

if (TEST)
    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
            ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
            EXCLUDE_FROM_ALL)

    # The gtest/gtest_main targets carry header search path
    # dependencies automatically when using CMake 2.8.11 or
    # later. Otherwise we have to add them here ourselves.
    if (CMAKE_VERSION VERSION_LESS 2.8.11)
        include_directories("${gtest_SOURCE_DIR}/include")
    endif()
    
    file (GLOB_RECURSE TEST_SOURCE "test/*.cpp" "test/*.c")
    add_executable(MXCoreTest ${TEST_SOURCE})
    target_link_libraries(MXCoreTest MXSchedule)
    target_link_libraries(MXCoreTest gtest_main)
endif(TEST)
