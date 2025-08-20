set(USER_LINKER_SCRIPT "${OCP_LRS_PATH}/target/zynq/cpu0/lscript.ld")

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/target/zynq/cpu0/main.c"
    "${OCP_LRS_PATH}/target/zynq/cpu0/tasks/blink.c"
    "${OCP_LRS_PATH}/target/zynq/cpu0/tasks/sysinit.c"
    "${OCP_LRS_PATH}/target/zynq/cpu0/tasks/uiface.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/target/zynq/cpu0/opilZynqCpu0.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/target/zynq/drivers/benchmarking_zynq.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_LRS_PATH}/target/zynq/ipc/ipcClientZynq.c"
)
