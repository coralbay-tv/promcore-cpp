#include "promcore/gauge.h"

#include <ctime>

namespace promcore {

GaugeDouble::GaugeDouble(const double value) : GaugeType(value) {}

GaugeInt::GaugeInt(const int64_t value) : GaugeType(value) {}

template <typename TYPE>
GaugeType<TYPE>::GaugeType(const TYPE value) : value_{value} {}

template <typename TYPE>
void GaugeType<TYPE>::Increment() { Increment(TYPE{1}); }

template <typename TYPE>
void GaugeType<TYPE>::Increment(const TYPE value) { Change(value); }

template <typename TYPE>
void GaugeType<TYPE>::Decrement() { Decrement(TYPE{1}); }

template <typename TYPE>
void GaugeType<TYPE>::Decrement(const TYPE value) { Change(TYPE{-1} * value); }

template <typename TYPE>
void GaugeType<TYPE>::Set(const TYPE value) { value_.store(value); }

template <typename TYPE>
void GaugeType<TYPE>::Change(const TYPE value) {
#if __cpp_lib_atomic_float >= 201711L
  value_.fetch_add(value);
#else
  // Pre-C++ 20 fallback: busy loop (which might be more expansive than using
  // fetch_add).
  auto current = value_.load();
  while (!value_.compare_exchange_weak(current, current + value)) {
    // intentionally empty block
  }
#endif
}

template <typename TYPE>
void GaugeType<TYPE>::SetToCurrentTime() {
  const auto time = std::time(nullptr);
  Set(static_cast<double>(time));
}

template <typename TYPE>
TYPE GaugeType<TYPE>::Value() const { return value_; }

template <typename TYPE>
ClientMetric GaugeType<TYPE>::Collect() const {
  ClientMetric metric;

  auto &gauge{metric.gauge};
  if constexpr (std::is_same_v<TYPE, double>) {
    gauge.type = ClientMetric::Gauge::Type::DOUBLE;
    gauge.value.d = Value();
  }
  else {
    gauge.type = ClientMetric::Gauge::Type::INT;
    gauge.value.i = Value();
  }

  return metric;
}

template class PROMCORE_CPP_CORE_EXPORT GaugeType<double>;
template class PROMCORE_CPP_CORE_EXPORT GaugeType<int64_t>;

}  // namespace promcore
