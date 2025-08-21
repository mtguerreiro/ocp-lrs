cmake_minimum_required(VERSION 3.18)

add_library(cuk)

target_sources(
    cuk
    PRIVATE
    cukController.c
    cukControlIdle.c
    cukControlRamp.c
    cukControlCascFblin.c
    cukControlEnergy.c
    cukControlEnergyMpc.c
    cukControlSfb.c
    cukHwIf.c
)


target_sources(
    cuk
    PRIVATE
    cdmpc/mvops.c
    cdmpc/dmpc_matrices.c
)

target_sources(
    cuk
    PRIVATE
    posix/cukHw.c
    posix/cuk.c
)
target_include_directories(
    cuk
    PUBLIC
    ./posix
)

target_link_libraries(
    cuk
    PRIVATE
    ocp
    opil
)

target_include_directories(
    cuk
    PRIVATE
    "${PROJECT_SOURCE_DIR}"
    ./
)
