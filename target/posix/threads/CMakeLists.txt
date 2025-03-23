
add_library(ocp_lrs_posix_threads)

target_sources(ocp_lrs_posix_threads
    PRIVATE
    ocpOpilThread.c
)

target_link_libraries(ocp_lrs_posix_threads
    PRIVATE
    ocp
    opil
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(ocp_lrs_posix_threads PRIVATE ocpIfLinuxThread.c)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(threads PRIVATE ocpIfWinThread.c)
    target_link_libraries(ocp_lrs_posix_threads PRIVATE ws2_32)
else ()
    message("-- THREADS: platform ${CMAKE_SYSTEM_NAME} not supported for this example.")
endif ()

target_include_directories(ocp_lrs_posix_threads
    PRIVATE
    "${PROJECT_SOURCE_DIR}"
)
