find_package(benchmark CONFIG REQUIRED HINTS /home/jask/install/)
find_package(GTest CONFIG REQUIRED HINTS /home/jask/install/)
include_directories(../deps/3rd/benchmark/include/)
set(BENCHMARK_ENABLE_SHARED
    OFF
    CACHE BOOL "Build benchmark as a static library")

include(GoogleTest)
include_directories(${PROJECT_SOURCE_DIR}/src/observer)

file(GLOB_RECURSE ALL_SRC *.cpp)
# AUX_SOURCE_DIRECTORY 类似功能
foreach(F ${ALL_SRC})
  get_filename_component(prjName ${F} NAME_WE)
  message("Build ${prjName} according to ${F}")
  add_executable(${prjName} ${F})
  target_link_libraries(${prjName} observer_static common pthread dl
                        benchmark::benchmark)
  if(NOT ${prjName} STREQUAL "memtracer_performance_test")
    target_link_libraries(${prjName} observer_static benchmark::benchmark)
  endif()

endforeach(F)
