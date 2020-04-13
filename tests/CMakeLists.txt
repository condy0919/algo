cmake_minimum_required(VERSION 3.11)

project(algo_tests LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.11.3
)
FetchContent_MakeAvailable(catch2)

add_executable(vector_unit_test
  vector_test.cpp
)

target_link_libraries(vector_unit_test
    algo
    Catch2
)

add_executable(stack_unit_test
  stack_test.cpp
)

target_link_libraries(stack_unit_test
  algo
  Catch2
)

add_test(test_all
  vector_unit_test
  stack_unit_test
)
