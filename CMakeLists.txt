cmake_minimum_required(VERSION 3.10)
project(MySTLTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# 查找 GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# 查找 tests 目录下的所有测试源文件
file(GLOB TEST_SOURCES "tests/*.cpp")

foreach (test_src ${TEST_SOURCES})
  get_filename_component(test_name ${test_src} NAME_WE)

  add_executable(${test_name} ${test_src})

  target_link_libraries(${test_name} gtest gtest_main pthread)

  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach ()
