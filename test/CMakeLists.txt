

include_directories(../include)
add_executable(byte_test bytes.c)
target_link_libraries(byte_test qcutils)

add_executable(io_test iotest.c)
target_link_libraries(io_test qcutils)


add_executable(list_test list.c)
target_link_libraries(list_test qcutils)


add_executable(log_test logtest.c)
target_link_libraries(log_test qcutils)

add_executable(map_test maptest.c)
target_link_libraries(map_test qcutils)

add_executable(rbtree_test rbtreeTest.c)
target_link_libraries(rbtree_test qcutils)

add_executable(qstr_test strtest.c)
target_link_libraries(qstr_test qcutils)



