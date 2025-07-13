cmake_minimum_required(VERSION 3.18)

include(BuildOptions.cmake)

if(FS_BUCK_BOOST)
    add_subdirectory(lrs_apps/fs_buck_boost)
endif()

add_subdirectory(target/posix/threads)
