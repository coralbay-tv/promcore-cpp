#include "promcore/counter.h"

namespace promcore {

template <typename TYPE>
void CounterType<TYPE>::Increment() { gauge_.Increment(); }

template <typename TYPE>
void CounterType<TYPE>::Increment(const TYPE val) {
  if (val < TYPE{0}) {
    return;
  }
  gauge_.Increment(val);
}

template <typename TYPE>
TYPE CounterType<TYPE>::Value() const { return gauge_.Value(); }

template <typename TYPE>
void CounterType<TYPE>::Reset() { gauge_.Set(0); }

template <typename TYPE>
ClientMetric CounterType<TYPE>::Collect() const {
  ClientMetric metric;

  auto &counter{metric.counter};
  if constexpr (std::is_same_v<TYPE, double>) {
    counter.type = ClientMetric::Counter::Type::DOUBLE;
    counter.value.d = Value();
  }
  else {
    counter.type = ClientMetric::Counter::Type::INT;
    counter.value.i = Value();
  }

  return metric;
}

template class CounterType<double>;
template class CounterType<int64_t>;

}  // namespace promcore
