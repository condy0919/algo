#include <benchmark/benchmark.h>

#include <vector>
#include <string>
#include "vector.hpp"

// std::vector
static void BM_std_vector_push_back_trivial(benchmark::State& state) {
    std::vector<int> vec;
    for (auto _ : state) {
        vec.push_back(42);
    }
}
BENCHMARK(BM_std_vector_push_back_trivial);

static void BM_std_vector_emplace_back_nontrivial(benchmark::State& state) {
    std::vector<std::string> vec;
    for (auto _ : state) {
        vec.emplace_back("this is a long string");
    }
}
BENCHMARK(BM_std_vector_emplace_back_nontrivial);

static void BM_std_vector_insert_front_trivial(benchmark::State& state) {
    std::vector<int> vec;
    for (auto _ : state) {
        vec.insert(vec.begin(), 42);
    }
}
BENCHMARK(BM_std_vector_insert_front_trivial);

static void BM_std_vector_insert_front_nontrivial(benchmark::State& state) {
    std::vector<std::string> vec;
    for (auto _ : state) {
        vec.insert(vec.begin(), "this is a long string");
    }
}
BENCHMARK(BM_std_vector_insert_front_nontrivial);

static void BM_std_vector_insert_back_trivial(benchmark::State& state) {
    std::vector<int> vec;
    for (auto _ : state) {
        vec.insert(vec.end(), 42);
    }
}
BENCHMARK(BM_std_vector_insert_back_trivial);

static void BM_std_vector_insert_back_nontrivial(benchmark::State& state) {
    std::vector<std::string> vec;
    for (auto _ : state) {
        vec.insert(vec.end(), "this is a long string");
    }
}
BENCHMARK(BM_std_vector_insert_back_nontrivial);

// algo::Vector
static void BM_algo_vector_push_back_trivial(benchmark::State& state) {
    algo::Vector<int> vec;
    for (auto _ : state) {
        vec.push_back(42);
    }
}
BENCHMARK(BM_algo_vector_push_back_trivial);

static void BM_algo_vector_emplace_back_nontrivial(benchmark::State& state) {
    algo::Vector<std::string> vec;
    for (auto _ : state) {
        vec.emplace_back("this is a long string");
    }
}
BENCHMARK(BM_algo_vector_emplace_back_nontrivial);

static void BM_algo_vector_insert_front_trivial(benchmark::State& state) {
    algo::Vector<int> vec;
    for (auto _ : state) {
        vec.insert(vec.begin(), 42);
    }
}
BENCHMARK(BM_algo_vector_insert_front_trivial);

static void BM_algo_vector_insert_front_nontrivial(benchmark::State& state) {
    algo::Vector<std::string> vec;
    for (auto _ : state) {
        vec.insert(vec.begin(), "this is a long string");
    }
}
BENCHMARK(BM_algo_vector_insert_front_nontrivial);

static void BM_algo_vector_insert_back_trivial(benchmark::State& state) {
    algo::Vector<int> vec;
    for (auto _ : state) {
        vec.insert(vec.end(), 42);
    }
}
BENCHMARK(BM_algo_vector_insert_back_trivial);

static void BM_algo_vector_insert_back_nontrivial(benchmark::State& state) {
    algo::Vector<std::string> vec;
    for (auto _ : state) {
        vec.insert(vec.end(), "this is a long string");
    }
}
BENCHMARK(BM_algo_vector_insert_back_nontrivial);

BENCHMARK_MAIN();
