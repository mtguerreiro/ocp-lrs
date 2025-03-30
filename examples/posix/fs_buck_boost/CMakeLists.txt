cmake_minimum_required(VERSION 3.18)

set(FS_BUCK_BOOST ON)
set(OCP_IF_THREAD_SERVER_PORT 8080)
set(OPIL_PORT 8090)

project(app_fsbb)

add_compile_definitions(TARGET_COMM_SOCK_SERVER_PORT=${OPIL_PORT})

set(OCP_LRS_PATH "../../../../ocp-lrs")
set(OCP_PATH "../../../../ocp")
set(OPIL_PATH "../../../../opil")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_compile_options(
    -Wall
    -Wextra
    -Wno-unused-parameter
    -O2
)

add_subdirectory(${OCP_LRS_PATH} "${PROJECT_BINARY_DIR}/ocp-lrs")
add_subdirectory(${OCP_PATH} "${PROJECT_BINARY_DIR}/ocp")
add_subdirectory(${OPIL_PATH} "${PROJECT_BINARY_DIR}/opil")

add_executable(app_fsbb main.c)

target_link_libraries(app_fsbb
    PRIVATE
    opil
    ocp
    ocp_lrs_posix_threads
    fs_buck_boost
)

target_include_directories(app_fsbb
    PRIVATE
    "${OCP_LRS_PATH}/"
)

