#include <benchmark/benchmark.h>

#include "promcore/counter.h"
#include "promcore/family.h"
#include "promcore/registry.h"

// Template for counter increment benchmark
template <typename BuildCounterT, typename CounterT>
void BM_Counter_Increment_Impl(benchmark::State& state) {
  using promcore::Registry;
  Registry registry;
  auto& counter_family =
      BuildCounterT().Name("benchmark_counter").Help("").Register(registry);
  auto& counter = counter_family.Add({});
  while (state.KeepRunning()) counter.Increment();
}

// Template for counter collect benchmark
template <typename BuildCounterT, typename CounterT>
void BM_Counter_Collect_Impl(benchmark::State& state) {
  using promcore::Registry;
  Registry registry;
  auto& counter_family =
      BuildCounterT().Name("benchmark_counter").Help("").Register(registry);
  auto& counter = counter_family.Add({});
  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(counter.Collect());
  };
}

// Instantiate benchmarks for Double counters
static void BM_Counter_Increment_Double(benchmark::State& state) {
  BM_Counter_Increment_Impl<decltype(promcore::BuildCounterDouble()), promcore::CounterDouble>(state);
}
BENCHMARK(BM_Counter_Increment_Double);

static void BM_Counter_Collect_Double(benchmark::State& state) {
  BM_Counter_Collect_Impl<decltype(promcore::BuildCounterDouble()), promcore::CounterDouble>(state);
}
BENCHMARK(BM_Counter_Collect_Double);

// Instantiate benchmarks for Int counters
static void BM_Counter_Increment_Int(benchmark::State& state) {
  BM_Counter_Increment_Impl<decltype(promcore::BuildCounterInt()), promcore::CounterInt>(state);
}
BENCHMARK(BM_Counter_Increment_Int);

static void BM_Counter_Collect_Int(benchmark::State& state) {
  BM_Counter_Collect_Impl<decltype(promcore::BuildCounterInt()), promcore::CounterInt>(state);
}
BENCHMARK(BM_Counter_Collect_Int);
