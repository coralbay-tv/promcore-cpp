# Prometheus Core Client Library for Modern C++

This library enables [Metrics-Driven Development (MDD)](https://sookocheff.com/post/mdd/mdd/) for C++ services.  
It implements the [Prometheus Data Model](https://prometheus.io/docs/concepts/data_model/), a powerful abstraction for collecting and exposing metrics.

**Credits:** Derived from [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp) under the same MIT license.

---

## ✨ Goals and Design Philosophy

`promcore-cpp` is a **leaner**, **simplified**, and **type-flexible** fork of `prometheus-cpp`, focused on core metric functionality and atomic performance.

### Key differences
- **Lightweight core:** The HTTP server, pull, and push components have been removed.
- **Core focus:** Only essential metric primitives remain (`Counter`, `Gauge`, `Histogram`, `Summary`, etc.).
- **Atomic/concurrency awareness:** For `Gauge` and `Counter`, both integer and floating-point variants are provided:
  - `CounterInt` → `std::atomic<int64_t>`
  - `CounterDouble` → `std::atomic<double>`

Use of integer atomics can lead to **significant performance improvements**, especially under contention.

---

## ⚙️ Atomic Type Choice: Integer vs Floating-Point

Choosing between integer and floating-point types for `Gauge` and `Counter` metrics has measurable performance implications.  
Examining the generated assembly (x86-64) shows why.

### Floating-Point Example (`std::atomic<double>`)

```cpp
void ChangeDouble(std::atomic<double>& atomic, double value) {
    atomic.fetch_add(value);
}
```

```asm
ChangeDouble(std::atomic<double>&, double):
    movq xmm1, QWORD PTR [rdi]
    lea  rdx, [rsp-8]
    movsd QWORD PTR [rsp-8], xmm1
    addsd xmm1, xmm0
.L2:
    mov  rax, QWORD PTR [rdx]
    movq rcx, xmm1
    lock cmpxchg QWORD PTR [rdi], rcx
    je   .L5
    mov  QWORD PTR [rdx], rax
    movsd xmm1, QWORD PTR [rsp-8]
    addsd xmm1, xmm0
    jmp  .L2
.L5:
    ret
```

This loop repeatedly performs a **compare-and-swap (CAS)** operation until successful — expensive under contention.

### Integer Example (`std::atomic<int64_t>`)

```cpp
void ChangeInt(std::atomic<int64_t>& atomic, int64_t value) {
    atomic.fetch_add(value);
}
```

```asm
ChangeInt(std::atomic<long>&, long):
    lock add QWORD PTR [rdi], rsi
    ret
```

Here, the `lock add` instruction performs the operation atomically in a single step — far more efficient than the CAS loop.

### 🧠 Takeaway

When precision requirements allow, **prefer integer atomics** for counters and gauges.  
Use floating-point atomics only when sub-integer increments are required.

### ARM/AArch64 Note

On ARMv8/AArch64, integer atomics also map efficiently to hardware instructions, while floating-point atomics may fall back to CAS loops or library calls.  
The same performance gap observed on x86 applies — often even more pronounced on ARM.

---

## 🚀 Usage Example

```cpp
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

  // Example HTTP server exposer (not packaged)
  Exposer exposer{"127.0.0.1:8080"};

  // Create a metrics registry (must stay alive)
  auto registry = std::make_shared<Registry>();

  // Define a counter family
  auto& packet_counter = BuildCounterInt()
                             .Name("observed_packets_total")
                             .Help("Number of observed packets")
                             .Register(*registry);

  // Create labeled metrics
  auto& tcp_rx = packet_counter.Add({{"protocol", "tcp"}, {"direction", "rx"}});
  auto& tcp_tx = packet_counter.Add({{"protocol", "tcp"}, {"direction", "tx"}});
  auto& udp_rx = packet_counter.Add({{"protocol", "udp"}, {"direction", "rx"}});
  auto& udp_tx = packet_counter.Add({{"protocol", "udp"}, {"direction", "tx"}});

  // Dynamic labels (low cardinality recommended)
  auto& http_requests = BuildCounterInt()
                            .Name("http_requests_total")
                            .Help("Number of HTTP requests")
                            .Register(*registry);

  exposer.RegisterCollectable(registry);

  for (;;) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    const auto rnd = std::rand();

    if (rnd & 1) tcp_rx.Increment();
    if (rnd & 2) tcp_tx.Increment();
    if (rnd & 4) udp_rx.Increment();
    if (rnd & 8) udp_tx.Increment();

    static const std::array<std::string, 4> methods = {"GET", "PUT", "POST", "HEAD"};
    http_requests.Add({{"method", methods[rnd % methods.size()]}}).Increment();
  }
}
```

---

## 🧱 Requirements

- **C++17** compliant compiler  
- Tested with **GCC 14** on **Ubuntu 24.04 (Noble Numbat)**

---

## 🛠️ Building

Supported build systems:
- [CMake](https://cmake.org)
- [Bazel](https://bazel.io)

### Using CMake

```bash
# fetch dependencies
git submodule update --init

mkdir _build && cd _build
cmake .. -DBUILD_SHARED_LIBS=ON
cmake --build . --parallel $(nproc)

# optional: run tests
ctest -V

# optional: install
cmake --install .
```

### Using Bazel

Requires Bazel with **Bzlmod (Modules)** support:

```bash
bazel test //...
```

---

## 📦 Packaging

You can generate `.deb` or `.rpm` packages using [CPack](https://cmake.org/cmake/help/latest/module/CPack.html):

```bash
cmake -B_build -DCPACK_GENERATOR=DEB -DBUILD_SHARED_LIBS=ON
cmake --build _build --target package --parallel $(nproc)
```

---

## 🔗 Consuming via CMake

Example importable project is available in  
`cmake/project-import-cmake/`.

---

## 🤝 Contributing

- Follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).
- Run `clang-format` before submitting PRs.
- Use [good commit messages](https://chris.beams.io/posts/git-commit/).

```bash
bazel build //...
bazel test //...
```

---

## 📊 Benchmarks

A benchmark suite is included.  
The benchmarks **do not attempt to introduce high contention**, so results reflect uncontended performance.

```bash
bazel run -c opt //core/benchmarks
```

Sample output (GCC 14, 20-core CPU):

```
BM_Counter_Increment_Double    16.5 ns
BM_Counter_Increment_Int       13.2 ns
...
```

---

## 📈 Project Status

⚠️ **Unstable**, but already deployed in production environments.

---

## ❓ FAQ

### What exposition formats are supported?

Only the [Prometheus Text Exposition Format](https://github.com/prometheus/docs/blob/master/content/docs/instrumenting/exposition_formats.md#text-format-details).

---

## 🪪 License

**MIT License**
