set(SOURCES
    execution_manager.cpp
    executor_update.cpp
    executor_insert.cpp
    execution_sort.cpp
    executor_delete.cpp
    executor_index_scan.cpp
    executor_seq_scan.cpp
    executor_nestedloop_join.cpp
    executor_projection.cpp)
add_library(execution OBJECT ${SOURCES})

target_link_libraries(execution system record transaction planner common)
