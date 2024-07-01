add_executable(create_table_test create_table_test2.cpp)
# add_library(test STATIC SOURCES)
target_link_libraries(create_table_test system gtest_main record storage fmt)

add_executable(lru_replacer_test lru_replacer_test.cpp)
target_link_libraries(lru_replacer_test system gtest_main record storage)

add_executable(disk_manager_test disk_manager_test.cpp)
target_link_libraries(disk_manager_test system gtest_main record storage)

add_executable(err_msg_test err_msg_test.cpp)
target_link_libraries(err_msg_test gtest_main)

#
# add_executable(unit_test unit_test.cpp) target_link_libraries(unit_test
# gtest_main system lru_replacer  storage )
add_executable(buffer_pool_manager_test buffer_pool_manager_test.cpp)
target_link_libraries(buffer_pool_manager_test gtest_main storage record)

add_executable(binder_test binder_test.cpp)
target_link_libraries(
  binder_test
  duckdb_pg_query
  fmt
  common
  gtest_main
  storage
  record
  binder
  lru_replacer
  system
  statement)
add_executable(fmt_test fmt_test.cpp)
target_link_libraries(fmt_test gtest_main fmt)
