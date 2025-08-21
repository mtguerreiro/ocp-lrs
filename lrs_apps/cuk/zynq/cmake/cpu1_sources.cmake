list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/lrs_apps/cuk/zynq/ocpZynqCpu1.c"
    "${OCP_LRS_PATH}/lrs_apps/cuk/zynq/cuk.c"
    "${OCP_LRS_PATH}/lrs_apps/cuk/zynq/cukHw.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/lrs_apps/cuk/cukController.c"
    "${OCP_LRS_PATH}/lrs_apps/cuk/cukControlIdle.c"
    "${OCP_LRS_PATH}/lrs_apps/cuk/cukControlRamp.c"
    "${OCP_LRS_PATH}/lrs_apps/cuk/cukControlEnergy.c"
    "${OCP_LRS_PATH}/lrs_apps/cuk/cukControlEnergyMpc.c"
    "${OCP_LRS_PATH}/lrs_apps/cuk/cukControlSfb.c"
    "${OCP_LRS_PATH}/lrs_apps/cuk/cukControlCascFblin.c"
    "${OCP_LRS_PATH}/lrs_apps/cuk/cukHwIf.c"
)


list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/lrs_apps/cuk/cdmpc/mvops.c"
    "${OCP_LRS_PATH}/lrs_apps/cuk/cdmpc/dmpc_matrices.c"
)
