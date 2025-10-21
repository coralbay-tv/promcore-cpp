#include "promcore/detail/builder.h"

#include "promcore/counter.h"
#include "promcore/detail/core_export.h"
#include "promcore/gauge.h"
#include "promcore/histogram.h"
#include "promcore/info.h"
#include "promcore/registry.h"
#include "promcore/summary.h"

namespace promcore {

namespace detail {

template <typename T>
Builder<T>& Builder<T>::Labels(const ::promcore::Labels& labels) {
  labels_ = labels;
  return *this;
}

template <typename T>
Builder<T>& Builder<T>::Name(const std::string& name) {
  name_ = name;
  return *this;
}

template <typename T>
Builder<T>& Builder<T>::Help(const std::string& help) {
  help_ = help;
  return *this;
}

template <typename T>
Family<T>& Builder<T>::Register(Registry& registry) {
  return registry.Add<T>(name_, help_, labels_);
}

template class PROMCORE_CPP_CORE_EXPORT Builder<CounterDouble>;
template class PROMCORE_CPP_CORE_EXPORT Builder<CounterInt>;
template class PROMCORE_CPP_CORE_EXPORT Builder<GaugeDouble>;
template class PROMCORE_CPP_CORE_EXPORT Builder<GaugeInt>;
template class PROMCORE_CPP_CORE_EXPORT Builder<Histogram>;
template class PROMCORE_CPP_CORE_EXPORT Builder<Info>;
template class PROMCORE_CPP_CORE_EXPORT Builder<Summary>;

}  // namespace detail

detail::Builder<CounterDouble> BuildCounterDouble() { return {}; }
detail::Builder<CounterInt> BuildCounterInt() { return {}; }
detail::Builder<GaugeDouble> BuildGaugeDouble() { return {}; }
detail::Builder<GaugeInt> BuildGaugeInt() { return {}; }
detail::Builder<Histogram> BuildHistogram() { return {}; }
detail::Builder<Info> BuildInfo() { return {}; }
detail::Builder<Summary> BuildSummary() { return {}; }

}  // namespace promcore
