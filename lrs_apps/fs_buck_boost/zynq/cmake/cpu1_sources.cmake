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
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlBuckPlecs.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlBoostEnergy.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlBoostEnergyMpc.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostHwIf.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/plecs/Buck_controller.c"
)
