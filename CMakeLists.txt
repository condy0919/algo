cmake_minimum_required(VERSION 3.11)

project(algo
  VERSION 0.1.0
  LANGUAGES CXX
  DESCRIPTION "A header only c++ library"
  HOMEPAGE_URL "https://github.com/condy0919/algo"
)

enable_testing()

option(ENABLE_TESTS "Run Unit tests" On)
option(ENABLE_BENCHMARK "Run benchmarks" Off)

add_subdirectory(src)

if (ENABLE_TESTS)
  add_subdirectory(tests)
endif()

if (ENABLE_BENCHMARK)
  add_subdirectory(benchmark)
endif()
