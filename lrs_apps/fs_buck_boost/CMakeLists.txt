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
    cdmpc/dmpc.c
    cdmpc/dmpc_matrices.c
    cdmpc/mvops.c
    cdmpc/qp.c
    cdmpc/osqp/auxil.c
    cdmpc/osqp/error.c
    cdmpc/osqp/lin_alg.c
    cdmpc/osqp/osqp.c
    cdmpc/osqp/proj.c
    cdmpc/osqp/qdldl.c
    cdmpc/osqp/qdldl_interface.c
    cdmpc/osqp/scaling.c
    cdmpc/osqp/util.c
    cdmpc/osqp/workspace.c
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
