
list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/target/zynq/cpu1/main.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/target/zynq/drivers/zynqAxiAdc.c"
    "${OCP_LRS_PATH}/target/zynq/drivers/zynqAxiPwm.c"
    "${OCP_LRS_PATH}/target/zynq/drivers/zynqAxiFsPwm.c"
    "${OCP_LRS_PATH}/target/zynq/drivers/benchmarking_zynq.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/target/zynq/ipc/ipcServerZynq.c"
)

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
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlBoostEnergy.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostControlBoostEnergyMpc.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/fsbuckboostHwIf.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/dmpc.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/dmpc_matrices.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/mvops.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/qp.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/osqp/auxil.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/osqp/error.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/osqp/lin_alg.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/osqp/osqp.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/osqp/proj.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/osqp/qdldl.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/osqp/qdldl_interface.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/osqp/scaling.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/osqp/util.c"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/cdmpc/osqp/workspace.c"
)
