# 可以获取父cmake的变量
message("${CMAKE_COMMON_FLAGS}")

find_package(GTest CONFIG REQUIRED HINTS /home/jask/install/ NO_DEFAULT_PATH)

add_subdirectory(common)
if(WITH_MEMTRACER)
  add_subdirectory(memtracer)
endif(WITH_MEMTRACER)

add_subdirectory(observer)
