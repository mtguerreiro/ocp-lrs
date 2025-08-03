cmake_minimum_required(VERSION 3.18)

add_library(fs_buck_boost)

target_sources(fs_buck_boost
    PRIVATE
    fsbuckboostController.c
    fsbuckboostControlIdle.c
    fsbuckboostControlRamp.c
    fsbuckboostControlCpl.c
    fsbuckboostControlBuckSfb.c
    fsbuckboostControlBoostEnergy.c
    fsbuckboostControlBoostEnergyMpc.c
    fsbuckboostHwIf.c
)

target_sources(fs_buck_boost
    PRIVATE
    posix/fsbuckboostHw.c
    posix/fsbuckboost.c
)
target_include_directories(fs_buck_boost
    PUBLIC
    ./posix
)

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
