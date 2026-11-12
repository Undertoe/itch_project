#include "itch/sentinel.hh"

#include <benchmark/benchmark.h>

static void BM_SentinelValue(benchmark::State& state) {
    for (auto _ : state) {
        benchmark::DoNotOptimize(itch::sentinel_value());
    }
}
BENCHMARK(BM_SentinelValue);
