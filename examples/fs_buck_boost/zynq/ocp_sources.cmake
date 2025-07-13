
list(APPEND USER_COMPILE_SOURCES
    "${OCP_PATH}/ocp/ocpCS.c"
    "${OCP_PATH}/ocp/ocpTrace.c"
    "${OCP_PATH}/ocp/ocpIf.c"
    "${OCP_PATH}/ocp/ocpOpil.c"
    "${OCP_PATH}/ocp/ocpPlatform.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_PATH}/trace/ctrace.c"
    "${OCP_PATH}/trace/ctrace_circBuf.c"
    "${OCP_PATH}/trace/ctrace_mem.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_PATH}/controlsys/controlsys.c"
    "${OCP_PATH}/controller/controller.c"
    "${OCP_PATH}/controller/controllerIf.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_PATH}/rp/rp.c"
    "${OCP_PATH}/utils/dfilt.c"
)

list(APPEND USER_COMPILE_SOURCES
    "${OCP_PATH}/ipc/ipcClient.c"
    "${OCP_PATH}/ipc/ipcServer.c"
)
