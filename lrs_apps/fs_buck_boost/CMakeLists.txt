cmake_minimum_required(VERSION 3.18)

add_library(fs_buck_boost)

set(GRAMPC_ROOT "E:/BOOST-converter/grampc")

target_sources(fs_buck_boost
    PRIVATE
    fsbuckboostController.c
    fsbuckboostControlIdle.c
    fsbuckboostControlRamp.c
    fsbuckboostControlCpl.c
    fsbuckboostControlBuckSfb.c
    fsbuckboostControlPlecs.c
    fsbuckboostControlBoostEnergy.c
    fsbuckboostControlBoostEnergyMpc.c
    fsbuckboostControlBoostCascFblin.c
    fsbuckboostControlBoostNMPC.c
    fsbuckboostHwIf.c
)

target_sources(fs_buck_boost
    PRIVATE
    plecs/Plecs_controller.c
)
target_sources(fs_buck_boost
    PRIVATE
    plecs/Plecs_controller.c
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
    "${GRAMPC_ROOT}/include"
)
target_sources(fs_buck_boost
    PRIVATE

    ${GRAMPC_ROOT}/src/discrete.c
    ${GRAMPC_ROOT}/src/grampc_alloc.c
    ${GRAMPC_ROOT}/src/grampc_configreader.c
    ${GRAMPC_ROOT}/src/grampc_erk.c
    ${GRAMPC_ROOT}/src/grampc_fixedsize.c
    ${GRAMPC_ROOT}/src/grampc_init.c
    ${GRAMPC_ROOT}/src/grampc_mess.c
    ${GRAMPC_ROOT}/src/grampc_run.c
    ${GRAMPC_ROOT}/src/grampc_setopt.c
    ${GRAMPC_ROOT}/src/grampc_setparam.c
    ${GRAMPC_ROOT}/src/grampc_util.c
    ${GRAMPC_ROOT}/src/rodas.c
    ${GRAMPC_ROOT}/src/ruku45.c
    ${GRAMPC_ROOT}/src/simpson.c
    ${GRAMPC_ROOT}/src/timing.c
    ${GRAMPC_ROOT}/src/trapezoidal.c
    ${GRAMPC_ROOT}/src/finite_diff.c
    probfct_PMSM.c
)