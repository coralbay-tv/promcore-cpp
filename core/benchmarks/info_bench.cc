#include <benchmark/benchmark.h>

#include "promcore/family.h"
#include "promcore/info.h"
#include "promcore/registry.h"

static void BM_Info_Collect(benchmark::State& state) {
  using promcore::BuildInfo;
  using promcore::Info;
  using promcore::Registry;
  Registry registry;
  auto& info_family =
      BuildInfo().Name("benchmark_info").Help("").Register(registry);
  auto& info = info_family.Add({});

  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(info.Collect());
  };
}
BENCHMARK(BM_Info_Collect);
