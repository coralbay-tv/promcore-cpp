
# Prometheus Core Client Library for Modern C++

This library aims to enable
[Metrics-Driven Development](https://sookocheff.com/post/mdd/mdd/) for
C++ services. It implements the
[Prometheus Data Model](https://prometheus.io/docs/concepts/data_model/),
a powerful abstraction on which to collect and expose metrics. We
offer the possibility for metrics to be collected by Prometheus, but
other push/pull collections can be added as plugins.

**Credits**: This is a derived work of [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp) that has adopted the same MIT license.

This project aims to be a leaner, simplified, more type-flexible fork of `prometheus-cpp`. This reflected in the following:
-   **Simplicity** / **lightweightness**: The server, pull, and push features have been removed.
-   **Core focus** : Only the essentials ("core") functionality remains
-   **Atomic / concurrency awareness**: For `Gauge` and `Counter` types, there are two types - CounterInt (`std::atomic<int64_t>`) and CounterDouble (`std::atomic<double>`).  Use of `std::atomic<int64_t>` can result in significant performance improvements.


## Usage
``` c++
#include <promcore/counter.h>
#include <promcore/registry.h>

#include <array>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>

int main() {
  using namespace promcore;

  // Create an http server running on port 8080 (NOTE: not packaged)
  Exposer exposer{"127.0.0.1:8080"};

  // create a metrics registry
  // @note it's the users responsibility to keep the object alive
  auto registry = std::make_shared<Registry>();

  // add a new counter family to the registry (families combine values with the
  // same name, but distinct label dimensions)
  auto& packet_counter = BuildCounterInt()
                             .Name("observed_packets_total")
                             .Help("Number of observed packets")
                             .Register(*registry);

  // add and remember dimensional data, incrementing those is very cheap
  auto& tcp_rx_counter =
      packet_counter.Add({{"protocol", "tcp"}, {"direction", "rx"}});
  auto& tcp_tx_counter =
      packet_counter.Add({{"protocol", "tcp"}, {"direction", "tx"}});
  auto& udp_rx_counter =
      packet_counter.Add({{"protocol", "udp"}, {"direction", "rx"}});
  auto& udp_tx_counter =
      packet_counter.Add({{"protocol", "udp"}, {"direction", "tx"}});

  // add a counter whose dimensional data is not known at compile time
  // nevertheless dimensional values should only occur in low cardinality.
  auto& http_requests_counter = BuildCounterInt()
                                    .Name("http_requests_total")
                                    .Help("Number of HTTP requests")
                                    .Register(*registry);

  // ask the exposer to scrape the registry on incoming HTTP requests
  exposer.RegisterCollectable(registry);

  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    const auto random_value = std::rand();

    if (random_value & 1) { tcp_rx_counter.Increment(); }
    if (random_value & 2) { tcp_tx_counter.Increment(); }
    if (random_value & 4) { udp_rx_counter.Increment(); }
    if (random_value & 8) { udp_tx_counter.Increment(); }

    const std::array<std::string, 4> methods = {"GET", "PUT", "POST", "HEAD"};
    auto method = methods.at(random_value % methods.size());
    // dynamically calling Family<T>.Add() works but is slow and should be
    // avoided
    http_requests_counter.Add({{"method", method}}).Increment();
  }
  return 0;
}

```

## Requirements

Using `promcore-cpp` requires a C++17 compliant compiler. It has been successfully tested with GNU GCC 14 on Ubuntu Noble Numbat (24.04).

## Building

There are two supported ways to build
`promcore-cpp` - [CMake](https://cmake.org)
and [bazel](https://bazel.io). 

### With CMake

For CMake builds don't forget to fetch the submodules first - only required if you want to enable testing.

Then build as usual.

``` shell
# fetch third-party dependencies
git submodule init
git submodule update

mkdir _build
cd _build

# run cmake
cmake .. -DBUILD_SHARED_LIBS=ON

# build
cmake --build . --parallel 4

# run tests
ctest -V

# install the libraries and headers
cmake --install .
```

### With Bazel

Install a recent [bazel](https://www.bazel.io) version with Modules support.
To build and test promcore-cpp run:

```shell
bazel test //...
```

## Packaging

By configuring CPack you can generate an installer like a
Debian package (.deb) or RPM (.rpm) for the static or dynamic
libraries so they can be easily installed on
other systems.

Please refer to the [CPack](https://cmake.org/cmake/help/latest/module/CPack.html)
documentation for all available generators and their
configuration options.

To generate a Debian package you could follow these steps:

``` shell
# fetch third-party dependencies
git submodule update --init

# run cmake
cmake -B_build -DCPACK_GENERATOR=DEB -DBUILD_SHARED_LIBS=ON # or OFF for static libraries

# build and package
cmake --build _build --target package --parallel $(nproc)
```

This will place an appropriately named .deb in the
`_build` folder. To build a RPM package set the `CPACK_GENERATOR`
variable to `RPM`. 

## Consuming the installed project

### CMake

Consuming promcore-cpp via CMake is the preferred way because all the dependencies
between the three promcore-cpp libraries are handled correctly.

The `cmake/project-import` directory contains an
example project and minimal [CMakeLists.txt](cmake/project-import-cmake/CMakeLists.txt).

## Contributing

Please adhere to the [Google C++ Style
Guide](https://google.github.io/styleguide/cppguide.html). Make sure
to clang-format your patches before opening a PR. Also make sure to
adhere to [these commit message
guidelines](https://chris.beams.io/posts/git-commit/).

You can check out this repo and build the library using
``` bash
bazel build //...
```

Run the unit tests using
```
bazel test //...
```

## Benchmarks

There's a benchmark suite you can run:

```
bazel run -c opt //core/benchmarks

INFO: Analysed target //core/benchmarks:benchmarks (0 packages loaded, 0 targets configured).
INFO: Found 1 target...
Target //core/benchmarks:benchmarks up-to-date:
  bazel-bin/core/benchmarks/benchmarks
INFO: Elapsed time: 0.356s, Critical Path: 0.01s, Remote (0.00% of the time): [queue: 0.00%, setup: 0.00%, process: 0.00%]
INFO: 0 processes.
INFO: Build completed successfully, 1 total action
2025-10-22T14:03:03+01:00
Run on (20 X 3000 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1024 KiB (x10)
  L3 Unified 14080 KiB (x1)
Load Average: 4.26, 1.97, 1.50
--------------------------------------------------------------------------------------
Benchmark                                            Time             CPU   Iterations
--------------------------------------------------------------------------------------
BM_Counter_Increment_Double                       16.5 ns         16.5 ns     42054968
BM_Counter_Collect_Double                         9.56 ns         9.56 ns     73299386
BM_Counter_Increment_Int                          13.2 ns         13.2 ns     52904575
BM_Counter_Collect_Int                            10.0 ns         10.0 ns     72926709
BM_Gauge_Increment_Double                         15.8 ns         15.8 ns     44333307
BM_Gauge_Decrement_Double                         15.7 ns         15.7 ns     44425317
BM_Gauge_SetToCurrentTime_Double                  16.1 ns         16.1 ns     43386653
BM_Gauge_Collect_Double                           8.27 ns         8.27 ns     83805720
BM_Gauge_Increment_Int                            14.4 ns         14.4 ns     48566609
BM_Gauge_Decrement_Int                            14.4 ns         14.4 ns     48608446
BM_Gauge_SetToCurrentTime_Int                     16.4 ns         16.4 ns     42551647
BM_Gauge_Collect_Int                              8.26 ns         8.26 ns     84447928
BM_Histogram_Observe/0/manual_time                55.6 ns          107 ns     12605574
BM_Histogram_Observe/1/manual_time                55.5 ns          107 ns     12474853
BM_Histogram_Observe/8/manual_time                74.8 ns          126 ns      9181146
BM_Histogram_Observe/64/manual_time               99.8 ns          152 ns      7003372
BM_Histogram_Observe/512/manual_time               122 ns          174 ns      5741822
BM_Histogram_Observe/4096/manual_time              153 ns          204 ns      4551668
BM_Histogram_Collect/0                            59.8 ns         59.8 ns     11710090
BM_Histogram_Collect/1                            59.6 ns         59.6 ns     11636510
BM_Histogram_Collect/8                             104 ns          104 ns      6735514
BM_Histogram_Collect/64                            495 ns          495 ns      1411252
BM_Histogram_Collect/512                          3757 ns         3757 ns       188549
BM_Histogram_Collect/4096                        29661 ns        29661 ns        23521
BM_Info_Collect                                   7.87 ns         7.87 ns     88759379
BM_Registry_CreateFamily                           139 ns          139 ns      5003316
BM_Registry_CreateCounter/0                        153 ns          153 ns      4588378
BM_Registry_CreateCounter/1                       1384 ns         1383 ns       591391
BM_Registry_CreateCounter/8                       6612 ns         6611 ns       107663
BM_Registry_CreateCounter/64                     53827 ns        53826 ns        12967
BM_Registry_CreateCounter/512                   474567 ns       474523 ns         1515
BM_Registry_CreateCounter/4096                 4076109 ns      4076022 ns          172
BM_Summary_Observe/0/iterations:262144            6841 ns         6841 ns       262144
BM_Summary_Observe/1/iterations:262144            6986 ns         6985 ns       262144
BM_Summary_Observe/8/iterations:262144           11470 ns        11469 ns       262144
BM_Summary_Observe/64/iterations:262144          17763 ns        17762 ns       262144
BM_Summary_Collect/0/0                            31.0 ns         31.0 ns     22530964
BM_Summary_Collect/1/0                            87.8 ns         87.8 ns      7982677
BM_Summary_Collect/8/0                             333 ns          333 ns      2103192
BM_Summary_Collect/64/0                           2209 ns         2209 ns       319230
BM_Summary_Collect/0/1                            31.0 ns         31.0 ns     22579679
BM_Summary_Collect/1/1                            96.3 ns         96.3 ns      7250533
BM_Summary_Collect/8/1                             538 ns          538 ns      1301237
BM_Summary_Collect/64/1                          11192 ns        11192 ns        62616
BM_Summary_Collect/0/8                            30.7 ns         30.7 ns     22866014
BM_Summary_Collect/1/8                             127 ns          127 ns      5447969
BM_Summary_Collect/8/8                            1495 ns         1495 ns       466433
BM_Summary_Collect/64/8                          56779 ns        56777 ns        12417
BM_Summary_Collect/0/64                           30.7 ns         30.7 ns     22553213
BM_Summary_Collect/1/64                            264 ns          264 ns      2644717
BM_Summary_Collect/8/64                           8916 ns         8916 ns        79134
BM_Summary_Collect/64/64                        474437 ns       474394 ns         1473
BM_Summary_Collect/0/512                          30.7 ns         30.7 ns     22577752
BM_Summary_Collect/1/512                          1318 ns         1318 ns       983293
BM_Summary_Collect/8/512                         41442 ns        41442 ns        16620
BM_Summary_Collect/64/512                      2466024 ns      2465769 ns          282
BM_Summary_Collect/0/4096                         31.0 ns         31.0 ns     22694259
BM_Summary_Collect/1/4096                         1788 ns         1788 ns       335431
BM_Summary_Collect/8/4096                       108186 ns       108184 ns         6342
BM_Summary_Collect/64/4096                     6314509 ns      6314417 ns          112
BM_Summary_Collect/0/32768                        30.7 ns         30.7 ns     22591109
BM_Summary_Collect/1/32768                        3522 ns         3522 ns       210354
BM_Summary_Collect/8/32768                      310463 ns       310461 ns         2274
BM_Summary_Collect/64/32768                   17339706 ns     17339219 ns           41
BM_Summary_Collect/0/262144                       30.7 ns         30.7 ns     22717341
```

## Project Status
Unstable but used in production.

## FAQ

### What scrape formats do you support

Only the [Prometheus Text Exposition
Format](https://github.com/prometheus/docs/blob/master/content/docs/instrumenting/exposition_formats.md#text-format-details).

## License

MIT

