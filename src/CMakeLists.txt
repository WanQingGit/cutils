aux_source_directory(. QCUTILS_SRC)
list(REMOVE_ITEM QCUTILS_SRC ./mem_pool.c)
message(${QCUTILS_SRC})
include_directories(../include/cutils)
add_library(qcutils SHARED ${QCUTILS_SRC})

target_link_libraries(qcutils m)