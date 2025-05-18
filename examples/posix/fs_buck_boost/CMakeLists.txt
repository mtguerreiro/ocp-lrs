cmake_minimum_required(VERSION 3.18)

set(FS_BUCK_BOOST ON)

set(OCP_LRS_PATH "../../../../ocp-lrs")
set(OCP_PATH "../../../../ocp")
set(OCP_IF_THREAD_SERVER_PORT 8080)

set(OPIL_PATH "../../../../opil")
set(OPIL_TARGET_IP "127.0.0.1" CACHE STRING "IP of the OPiL target")
set(OPIL_TARGET_PORT 8090 CACHE STRING "Port opened by the OPiL target")

project(app_fsbb)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_compile_options(
    -Wall
    -Wextra
    -Wno-unused-parameter
    -O2
)

set(PLECS_BUILD_DIR "${CMAKE_SOURCE_DIR}/plecs/build/")
add_custom_target(build_plecs ALL
    COMMAND ${CMAKE_MAKE_PROGRAM} -f ${CMAKE_SOURCE_DIR}/plecs/make_opil_plecs.mk BUILD_DIR=${PLECS_BUILD_DIR} OPIL=${OPIL_PATH} HOST_COMM_SOCK_SERVER_IP="${OPIL_TARGET_IP}" HOST_COMM_SOCK_SERVER_PORT=${OPIL_TARGET_PORT} STYPES_DIR=${PROJECT_SOURCE_DIR}/
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building OPiL/PLECS DLL using external Makefile"
)

add_subdirectory(${OPIL_PATH} "${PROJECT_BINARY_DIR}/opil")
target_compile_definitions(
    opil
    PRIVATE
    TARGET_COMM_SOCK_SERVER_PORT=${OPIL_TARGET_PORT}
)

add_subdirectory(${OCP_LRS_PATH} "${PROJECT_BINARY_DIR}/ocp-lrs")
add_subdirectory(${OCP_PATH} "${PROJECT_BINARY_DIR}/ocp")

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

