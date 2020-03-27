# algo

Algorithms for dsa course.

## Vector

Optimized `vector` for `push_back()` and `emplace_back()` operations.

# Benchmark

## `algo::Vector` vs `std::vector`

| Operation                    | `std::vector` (ns) | `algo::Vector` (ns) |
|------------------------------|-------------------:|--------------------:|
| **push_back** trivial        |               7.06 |                1.89 |
| **emplace_back** non-trivial |               67.2 |                57.0 |
| insert_front trivial         |               5630 |               10054 |
| insert_front non-trivial     |             711333 |               62126 |
| insert_back trivial          |               5.60 |                4.33 |
| insert_back non-trivial      |               71.1 |                58.4 |
