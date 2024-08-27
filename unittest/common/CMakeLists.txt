# get_filename_component(<VAR> FileName PATH|ABSOLUTE|NAME|EXT|NAME_WE|REALPATH
# [CACHE])
file(GLOB_RECURSE ALL_SRC *.cpp)
find_package(GTest CONFIG REQUIRED HINTS /home/jask/install/ NO_DEFAULT_PATH)
# AUX_SOURCE_DIRECTORY 类似功能
foreach(F ${ALL_SRC})
  get_filename_component(prjName ${F} NAME_WE)
  message("Build ${prjName} according to ${F}")
  add_executable(${prjName} ${F})
  target_link_libraries(${prjName} common pthread dl GTest::gtest
                        GTest::gtest_main)
  add_test(NAME ${prjName} COMMAND ${prjName})
endforeach(F)
