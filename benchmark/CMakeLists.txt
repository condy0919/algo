cmake_minimum_required(VERSION 3.11)

project(algo_tests LANGUAGES CXX)

include(FetchContent)

set(BENCHMARK_ENABLE_TESTING Off)

FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.5.0
)
FetchContent_MakeAvailable(benchmark)

add_executable(vector_benchmark
  vector_benchmark.cpp
)

target_link_libraries(vector_benchmark
  algo
  benchmark
)

add_test(benchmark_all
  vector_benchmark
)
