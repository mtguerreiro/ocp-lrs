set(USER_LINKER_SCRIPT "${OCP_LRS_PATH}/target/zynq/cpu1/lscript.ld")

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
