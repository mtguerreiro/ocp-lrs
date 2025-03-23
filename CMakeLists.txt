cmake_minimum_required(VERSION 3.18)

include(BuildOptions.cmake)

if(FS_BUCK_BOOST)
    add_subdirectory(lrs_apps/fs_buck_boost)
endif()

if( CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Windows" )
    add_subdirectory(target/posix/threads)
endif()

