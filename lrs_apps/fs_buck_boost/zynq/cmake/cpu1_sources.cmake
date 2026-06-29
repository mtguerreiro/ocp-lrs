list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/zynq/ocpZynqCpu1.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/zynq/fsbuckboost.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/zynq/fsbuckboostHw.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostController.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlIdle.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlRamp.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlCpl.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlBuckSfb.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlPlecs.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlBoostEnergy.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlBoostEnergyMpc.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlBoostCascFblin.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlBoostNMPC.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostHwIf.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/probfct_PMSM.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/plecs/Plecs_controller.c"
)
set(GRAMPC_ROOT "D:/proj/grampc")

list(APPEND USER_COMPILE_SOURCES
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
)
