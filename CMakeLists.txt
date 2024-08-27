cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

project(minidb)
set(CMAKE_PREFIX_PATH /home/jask/install/)
message(STATUS "This is Project source dir " ${PROJECT_SOURCE_DIR})
message(STATUS "This is PROJECT_BINARY_DIR dir " ${PROJECT_BINARY_DIR})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

find_package(benchmark CONFIG REQUIRED HINTS /home/jask/install/)
include_directories(../deps/3rd/benchmark/include/)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

option(ENABLE_ASAN "Enable build with address sanitizer" ON)
option(ENABLE_TSAN "Build with thread sanitizer" OFF)
option(ENABLE_UBSAN "Build with undefined behavior sanitizer" OFF)
option(WITH_UNIT_TESTS "Compile miniob with unit tests" ON)
option(WITH_BENCHMARK "Compile benchmark" OFF)
# TODO: support MemTracer with sanitizers, currently MemTracer doesn't work with
# sanitizers.
option(WITH_MEMTRACER "Compile memtracer" ON)
option(ENABLE_COVERAGE "Enable unittest coverage" OFF)
option(ENABLE_NOPIE "Enable no pie" OFF)
option(CONCURRENCY "Support concurrency operations" OFF)
option(STATIC_STDLIB
       "Link std library static or dynamic, such as libgcc, libstdc++, libasan"
       OFF)
option(USE_SIMD "Use SIMD" OFF)

message(STATUS "HOME dir: $ENV{HOME}")
# SET(ENV{变量名} 值)
if(WIN32)
  message(STATUS "This is windows.")
  add_definitions(-DWIN32)
elseif(WIN64)
  message(STATUS "This is windows.")
  add_definitions(-DWIN64)
elseif(APPLE)
  message(STATUS "This is apple")
  # normally __MACH__ has already been defined
  add_definitions(-D__MACH__)
elseif(UNIX)
  message(STATUS "This is UNIX")
  add_definitions(-DUNIX -DLINUX)
else()
  message(STATUS "This is UNKNOW OS")
endif(WIN32)

# This is for clangd plugin for vscode mute sign-compare error in lex/yacc
set(CMAKE_COMMON_FLAGS
    "${CMAKE_COMMON_FLAGS} -Wall -Werror -Wno-error=sign-compare")
if(ENABLE_NOPIE)
  set(CMAKE_COMMON_FLAGS "${CMAKE_COMMON_FLAGS} -no-pie")
  add_link_options(-no-pie)
endif(ENABLE_NOPIE)

# Requires support for avx2
if(USE_SIMD)
  set(CMAKE_COMMON_FLAGS "${CMAKE_COMMON_FLAGS} -mavx2")
  add_definitions(-DUSE_SIMD)
endif(USE_SIMD)

if(DEBUG)
  message(STATUS "DEBUG has been set as TRUE ${DEBUG}")
  set(CMAKE_COMMON_FLAGS "${CMAKE_COMMON_FLAGS}  -O0 -g -DDEBUG ")
  add_definitions(-DENABLE_DEBUG)
elseif(NOT DEFINED ENV{DEBUG})
  message(STATUS "Disable debug")
  set(CMAKE_COMMON_FLAGS "${CMAKE_COMMON_FLAGS}  -O2 -g ")
else()
  message(STATUS "Enable debug")
  set(CMAKE_COMMON_FLAGS "${CMAKE_COMMON_FLAGS}  -O0 -g -DDEBUG")
  add_definitions(-DENABLE_DEBUG)
endif(DEBUG)

if(CONCURRENCY)
  message(STATUS "CONCURRENCY is ON")
  set(CMAKE_COMMON_FLAGS "${CMAKE_COMMON_FLAGS} -DCONCURRENCY")
  add_definitions(-DCONCURRENCY)
endif(CONCURRENCY)

message(STATUS "CMAKE_CXX_COMPILER_ID is " ${CMAKE_CXX_COMPILER_ID})
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND ${STATIC_STDLIB})
  add_link_options(-static-libgcc -static-libstdc++)
endif()

if(ENABLE_ASAN)
  message(STATUS "Instrumenting with Address Sanitizer")
  set(CMAKE_COMMON_FLAGS
      "${CMAKE_COMMON_FLAGS} -fno-omit-frame-pointer -fsanitize=address -fsanitize-address-use-after-scope"
  )
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND ${STATIC_STDLIB})
    add_link_options(-static-libasan)
  endif()
endif()

if(ENABLE_TSAN)
  # supported flags
  # https://github.com/google/sanitizers/wiki/ThreadSanitizerFlags
  message(STATUS "Instrumenting with Thread Sanitizer")
  set(TSAN_FLAGS "-fno-omit-frame-pointer -fsanitize=thread")
  set(TSAN_CCFLAGS "${TSAN_CCFLAGS} -mllvm -tsan-instrument-memory-accesses=0")
  set(TSAN_CCFLAGS "${TSAN_CCFLAGS} -mllvm -tsan-instrument-atomics=0")
  set(TSAN_CCFLAGS "${TSAN_CCFLAGS} -mllvm -tsan-instrument-func-entry-exit=1")
  set(CMAKE_COMMON_FLAGS "${CMAKE_COMMON_FLAGS} ${TSAN_FLAGS} ${TSAN_CCFLAGS}")
  # -Qunused-arguments 有些编译器不支持，所以先删掉
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND ${STATIC_STDLIB})
    add_link_options(-static-libtsan)
  endif()
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${TSAN_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${TSAN_FLAGS}")
endif(ENABLE_TSAN)

if(ENABLE_UBSAN)
  # Only success on Mac Clang
  message(STATUS "Instrumenting with Undefined Behavior Sanitizer")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fno-omit-frame-pointer")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=undefined")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=implicit-conversion")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=implicit-integer-truncation")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=integer")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=nullability")
  set(UBSAN_FLAGS "${UBSAN_FLAGS} -fsanitize=vptr")
  set(CMAKE_COMMON_FLAGS "${CMAKE_COMMON_FLAGS} ${UBSAN_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${UBSAN_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${UBSAN_FLAGS}")
endif(ENABLE_UBSAN)

if(CMAKE_INSTALL_PREFIX)
  message(STATUS "CMAKE_INSTALL_PREFIX has been set as "
                 ${CMAKE_INSTALL_PREFIX})
elseif(DEFINED ENV{CMAKE_INSTALL_PREFIX})
  set(CMAKE_INSTALL_PREFIX $ENV{CMAKE_INSTALL_PREFIX})
else()
  set(CMAKE_INSTALL_PREFIX /tmp/${PROJECT_NAME})
endif()
message(STATUS "Install target dir is " ${CMAKE_INSTALL_PREFIX})

if(DEFINED ENV{LD_LIBRARY_PATH})
  set(LD_LIBRARY_PATH_STR $ENV{LD_LIBRARY_PATH})
  string(REPLACE ":" ";" LD_LIBRARY_PATH_LIST ${LD_LIBRARY_PATH_STR})
  message(" Add LD_LIBRARY_PATH to -L flags " ${LD_LIBRARY_PATH_LIST})
  link_directories(${LD_LIBRARY_PATH_LIST})
endif()
link_directories(/home/jask/install/)
include_directories(. ${PROJECT_SOURCE_DIR}/deps /home/jask/install/)

if(WITH_UNIT_TESTS)
  if(ENABLE_COVERAGE)
    set(CMAKE_COMMON_FLAGS
        "${CMAKE_COMMON_FLAGS} -fprofile-arcs -ftest-coverage")
  endif(ENABLE_COVERAGE)
  enable_testing()
endif(WITH_UNIT_TESTS)

set(CMAKE_CXX_FLAGS ${CMAKE_COMMON_FLAGS})
set(CMAKE_C_FLAGS ${CMAKE_COMMON_FLAGS})
message(STATUS "CMAKE_CXX_FLAGS is " ${CMAKE_CXX_FLAGS})

# ADD_SUBDIRECTORY(src bin)  bin 为目标目录， 可以省略
add_subdirectory(deps)
add_subdirectory(src/obclient)
add_subdirectory(src/observer)
add_subdirectory(test/perf)
add_subdirectory(tools)

if(WITH_BENCHMARK)
  add_subdirectory(benchmark)
endif(WITH_BENCHMARK)

if(WITH_UNIT_TESTS)
  add_subdirectory(unittest)
endif(WITH_UNIT_TESTS)
