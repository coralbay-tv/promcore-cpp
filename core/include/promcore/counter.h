#pragma once

#include "promcore/client_metric.h"
#include "promcore/detail/builder.h"  // IWYU pragma: export
#include "promcore/detail/core_export.h"
#include "promcore/gauge.h"
#include "promcore/metric_type.h"

namespace promcore {

/// \brief A counter metric to represent a monotonically increasing value.
///
/// This class represents the metric type counter:
/// https://prometheus.io/docs/concepts/metric_types/#counter
///
/// The value of the counter can only increase. Example of counters are:
/// - the number of requests served
/// - tasks completed
/// - errors
///
/// Do not use a counter to expose a value that can decrease - instead use a
/// Gauge.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
template <typename TYPE>
class  CounterType {
public:

  using value_type = TYPE;

  static const MetricType metric_type{MetricType::Counter};

  /// \brief Create a counter that starts at 0.
  CounterType() = default;

  /// \brief Increment the counter by 1.
  void Increment();

  /// \brief Increment the counter by a given amount.
  ///
  /// The counter will not change if the given amount is negative.
  void Increment(TYPE);

  /// \brief Reset the counter to 0
  void Reset();

  /// \brief Get the current value of the counter.
  TYPE Value() const;

  /// \brief Get the current value of the counter.
  ///
  /// Collect is called by the Registry when collecting metrics.
  ClientMetric Collect() const;

private:
  GaugeType<TYPE> gauge_{0};
};

/// \brief A counter metric to represent a monotonically increasing value.
///
/// This class represents the metric type counter:
/// https://prometheus.io/docs/concepts/metric_types/#counter
///
/// The value of the counter can only increase. Example of counters are:
/// - the number of requests served
/// - tasks completed
/// - errors
///
/// Do not use a counter to expose a value that can decrease - instead use a
/// Gauge.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class CounterDouble : public CounterType<double>{
 public:

  using typename CounterType<double>::value_type;

  /// \brief Create a counter that starts at 0.
  CounterDouble() = default;
};

/// \brief A counter metric to represent a monotonically increasing value.
///
/// This class represents the metric type counter:
/// https://prometheus.io/docs/concepts/metric_types/#counter
///
/// The value of the counter can only increase. Example of counters are:
/// - the number of requests served
/// - tasks completed
/// - errors
///
/// Do not use a counter to expose a value that can decrease - instead use a
/// Gauge.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class CounterInt : public CounterType<int64_t> {
public:

  using typename CounterType<int64_t>::value_type;

  /// \brief Create a counter that starts at 0.
  CounterInt() = default;
};

/// \brief Return a builder to configure and register a Counter metric.
///
/// @copydetails Family<>::Family()
///
/// Example usage:
///
/// \code
/// auto registry = std::make_shared<Registry>();
/// auto& counter_family = prometheus::BuildCounter()
///                            .Name("some_name")
///                            .Help("Additional description.")
///                            .Labels({{"key", "value"}})
///                            .Register(*registry);
///
/// ...
/// \endcode
///
/// \return An object of unspecified type T, i.e., an implementation detail
/// except that it has the following members:
///
/// - Name(const std::string&) to set the metric name,
/// - Help(const std::string&) to set an additional description.
/// - Labels(const Labels&) to assign a set of
///   key-value pairs (= labels) to the metric.
///
/// To finish the configuration of the Counter metric, register it with
/// Register(Registry&).
PROMCORE_CPP_CORE_EXPORT detail::Builder<CounterDouble> BuildCounterDouble();
PROMCORE_CPP_CORE_EXPORT detail::Builder<CounterInt> BuildCounterInt();

}  // namespace promcore
