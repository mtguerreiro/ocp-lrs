cmake_minimum_required(VERSION 3.18)

add_library(fs_buck_boost)

target_sources(fs_buck_boost
    PRIVATE
    fsbuckboostController.c
    fsbuckboostControlIdle.c
    fsbuckboostControlRamp.c
    fsbuckboostControlBuckSfb.c
    fsbuckboostControlBoostEnergy.c

    fsbuckboostHwIf.c
)

if( CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Windows" )
    target_sources(fs_buck_boost
        PRIVATE
        posix/fsbuckboostHw.c
        posix/fsbuckboost.c
    )
    target_include_directories(fs_buck_boost
        PUBLIC
        ./posix
    )
elseif( CMAKE_SYSTEM_NAME STREQUAL "ZYNQ" )
    target_sources(fs_buck_boost
    PRIVATE
    zynq/fsbuckboostHw.c
    zynq/fsbuckboost.c
    )
    target_include_directories(fs_buck_boost
        PUBLIC
        ./zynq
    )
endif()

target_link_libraries(fs_buck_boost
    PRIVATE
    ocp
    opil
)

target_include_directories(fs_buck_boost
    PRIVATE
    "${PROJECT_SOURCE_DIR}"
    ./
)
