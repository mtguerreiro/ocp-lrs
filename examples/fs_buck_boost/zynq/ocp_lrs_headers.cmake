
list(APPEND USER_INCLUDE_DIRECTORIES
    "${OCP_LRS_PATH}/target/zynq/cpu0"
    "${OCP_LRS_PATH}/target/zynq/cpu0/tasks"
    )

list(APPEND USER_INCLUDE_DIRECTORIES
    "${OCP_LRS_PATH}/target/zynq/drivers"
    "${OCP_LRS_PATH}/target/zynq/ipc"
)

list(APPEND USER_INCLUDE_DIRECTORIES
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost"
    "${OCP_LRS_PATH}/lrs_apps/fs_buck_boost/zynq"
)
