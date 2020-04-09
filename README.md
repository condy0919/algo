[![Build Status][github-ci-badge]][github-link]
[![MIT License][license-badge]](LICENSE)
[![Language][language-badge]][language-link]

# algo

Algorithms for dsa course.

## Vector

Optimized `vector` for `push_back()` and `emplace_back()` operations.

# Benchmark

Run

``` shell
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release -DENABLE_BENCHMARK=On
cmake --build build
```

to build all benchmarks.

## `algo::Vector` vs `std::vector`

Run

``` shell
./build/benchmark/vector_benchmark
```

to get the result.

| Operation                    | `std::vector` (ns) | `algo::Vector` (ns) |
|------------------------------|-------------------:|--------------------:|
| **push_back** trivial        |               7.64 |                1.93 |
| **emplace_back** non-trivial |               72.8 |                81.2 |
| insert_front trivial         |               6056 |                8177 |
| insert_front non-trivial     |              76253 |               78311 |
| insert_back trivial          |               6.52 |                5.60 |
| insert_back non-trivial      |               71.3 |                73.5 |

[github-link]: https://github.com/condy0919/algo
[github-ci-badge]: https://github.com/condy0919/algo/workflows/CI/badge.svg
[language-link]: https://en.cppreference.com/w/cpp/compiler_support
[language-badge]: https://img.shields.io/badge/Language-C%2B%2B17-blue.svg
[license-badge]: https://img.shields.io/badge/license-MIT-007EC7.svg
