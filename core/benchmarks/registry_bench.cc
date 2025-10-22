#include <benchmark/benchmark.h>

#include <chrono>

#include "benchmark_helpers.h"
#include "promcore/counter.h"
#include "promcore/family.h"
#include "promcore/registry.h"

static void BM_Registry_CreateFamily(benchmark::State& state) {
  using promcore::BuildCounterInt;
  using promcore::CounterInt;
  using promcore::Registry;
  Registry registry;

  while (state.KeepRunning())
    BuildCounterInt().Name("benchmark_counter").Help("").Register(registry);
}
BENCHMARK(BM_Registry_CreateFamily);

static void BM_Registry_CreateCounter(benchmark::State& state) {
  using promcore::BuildCounterInt;
  using promcore::CounterInt;
  using promcore::Registry;
  Registry registry;
  auto& counter_family = BuildCounterInt()
                             .Labels(GenerateRandomLabels(10))
                             .Name("benchmark_counter")
                             .Help("")
                             .Register(registry);

  while (state.KeepRunning()) {
    auto labels = GenerateRandomLabels(state.range(0));

    auto start = std::chrono::high_resolution_clock::now();
    counter_family.Add(labels);
    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed_seconds =
        std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    state.SetIterationTime(elapsed_seconds.count());
  }
}
BENCHMARK(BM_Registry_CreateCounter)->Range(0, 4096);
