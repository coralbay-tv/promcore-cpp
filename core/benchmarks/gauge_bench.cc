#include <benchmark/benchmark.h>

#include "promcore/family.h"
#include "promcore/gauge.h"
#include "promcore/registry.h"

// Template for gauge increment benchmark
template <typename BuildGaugeT, typename GaugeT>
void BM_Gauge_Increment_Impl(benchmark::State& state) {
  using promcore::Registry;
  Registry registry;
  auto& gauge_family =
      BuildGaugeT().Name("benchmark_gauge").Help("").Register(registry);
  auto& gauge = gauge_family.Add({});
  while (state.KeepRunning()) {
    gauge.Increment(2);
    benchmark::DoNotOptimize(gauge);
  }
}

// Template for gauge decrement benchmark
template <typename BuildGaugeT, typename GaugeT>
void BM_Gauge_Decrement_Impl(benchmark::State& state) {
  using promcore::Registry;
  Registry registry;
  auto& gauge_family =
      BuildGaugeT().Name("benchmark_gauge").Help("").Register(registry);
  auto& gauge = gauge_family.Add({});
  while (state.KeepRunning()) {
    gauge.Decrement(2);
    benchmark::DoNotOptimize(gauge);
  }
}

// Template for gauge SetToCurrentTime benchmark
template <typename BuildGaugeT, typename GaugeT>
void BM_Gauge_SetToCurrentTime_Impl(benchmark::State& state) {
  using promcore::Registry;
  Registry registry;
  auto& gauge_family =
      BuildGaugeT().Name("benchmark_gauge").Help("").Register(registry);
  auto& gauge = gauge_family.Add({});
  while (state.KeepRunning()) {
    gauge.SetToCurrentTime();
    benchmark::DoNotOptimize(gauge);
  }
}

// Template for gauge collect benchmark
template <typename BuildGaugeT, typename GaugeT>
void BM_Gauge_Collect_Impl(benchmark::State& state) {
  using promcore::Registry;
  Registry registry;
  auto& gauge_family =
      BuildGaugeT().Name("benchmark_gauge").Help("").Register(registry);
  auto& gauge = gauge_family.Add({});
  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(gauge.Collect());
  };
}

// Instantiate benchmarks for Double gauges
static void BM_Gauge_Increment_Double(benchmark::State& state) {
  BM_Gauge_Increment_Impl<decltype(promcore::BuildGaugeDouble()), promcore::GaugeDouble>(state);
}
BENCHMARK(BM_Gauge_Increment_Double);

static void BM_Gauge_Decrement_Double(benchmark::State& state) {
  BM_Gauge_Decrement_Impl<decltype(promcore::BuildGaugeDouble()), promcore::GaugeDouble>(state);
}
BENCHMARK(BM_Gauge_Decrement_Double);

static void BM_Gauge_SetToCurrentTime_Double(benchmark::State& state) {
  BM_Gauge_SetToCurrentTime_Impl<decltype(promcore::BuildGaugeDouble()), promcore::GaugeDouble>(state);
}
BENCHMARK(BM_Gauge_SetToCurrentTime_Double);

static void BM_Gauge_Collect_Double(benchmark::State& state) {
  BM_Gauge_Collect_Impl<decltype(promcore::BuildGaugeDouble()), promcore::GaugeDouble>(state);
}
BENCHMARK(BM_Gauge_Collect_Double);

// Instantiate benchmarks for Int gauges
static void BM_Gauge_Increment_Int(benchmark::State& state) {
  BM_Gauge_Increment_Impl<decltype(promcore::BuildGaugeInt()), promcore::GaugeInt>(state);
}
BENCHMARK(BM_Gauge_Increment_Int);

static void BM_Gauge_Decrement_Int(benchmark::State& state) {
  BM_Gauge_Decrement_Impl<decltype(promcore::BuildGaugeInt()), promcore::GaugeInt>(state);
}
BENCHMARK(BM_Gauge_Decrement_Int);

static void BM_Gauge_SetToCurrentTime_Int(benchmark::State& state) {
  BM_Gauge_SetToCurrentTime_Impl<decltype(promcore::BuildGaugeInt()), promcore::GaugeInt>(state);
}
BENCHMARK(BM_Gauge_SetToCurrentTime_Int);

static void BM_Gauge_Collect_Int(benchmark::State& state) {
  BM_Gauge_Collect_Impl<decltype(promcore::BuildGaugeInt()), promcore::GaugeInt>(state);
}
BENCHMARK(BM_Gauge_Collect_Int);
