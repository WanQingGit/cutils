aux_source_directory(. QLIBC_SRC)
list(REMOVE_ITEM QLIBC_SRC ./mem_pool.c)
message(${QLIBC_SRC})
include_directories(../include/qlibc)
add_library(qlibc SHARED ${QLIBC_SRC})

target_link_libraries(qlibc m)