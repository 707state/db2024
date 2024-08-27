message(STATUS "This is CMAKE_CURRENT_SOURCE_DIR dir "
               ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_PREFIX_PATH /home/jask/install/)
file(GLOB_RECURSE ALL_SRC *.cpp *.c)
set(MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
message("MAIN SRC: " ${MAIN_SRC})
foreach(F ${ALL_SRC})

  if(NOT ${F} STREQUAL ${MAIN_SRC})
    set(LIB_SRC ${LIB_SRC} ${F})
    message("Use " ${F})
  endif()

endforeach(F)

set(LIBEVENT_STATIC_LINK TRUE)
find_package(
  Libevent
  2.1
  CONFIG
  REQUIRED
  PATHS
  /home/jask/install/
  NO_DEFAULT_PATH)

# JsonCpp cannot work correctly with FIND_PACKAGE

set(LIBRARIES common pthread dl libevent::core libevent::pthreads libjsoncpp.a)

# 指定目标文件位置
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
message("Binary directory:" ${EXECUTABLE_OUTPUT_PATH})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
message("Archive directory:" ${LIBRARY_OUTPUT_PATH})

add_executable(observer ${MAIN_SRC})
target_link_libraries(observer observer_static)

add_library(observer_static STATIC ${LIB_SRC})
include(readline)
miniob_find_readline()
if(HAVE_READLINE)
  target_link_libraries(observer_static ${READLINE_LIBRARY})
  target_include_directories(observer_static PRIVATE ${READLINE_INCLUDE_DIR})
  add_definitions(-DUSE_READLINE)
  message("observer_static use readline")
else()
  message("readline is not found")
endif()

set_target_properties(observer_static PROPERTIES OUTPUT_NAME observer)
target_link_libraries(observer_static ${LIBRARIES})

# Target 必须在定义 ADD_EXECUTABLE 之后， programs 不受这个限制 TARGETS和PROGRAMS
# 的默认权限是OWNER_EXECUTE, GROUP_EXECUTE, 和WORLD_EXECUTE，即755权限， programs 都是处理脚本类
# 类型分为RUNTIME／LIBRARY／ARCHIVE, prog
install(
  TARGETS observer observer_static
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib)
