#pragma once

#include <atomic>

#include "promcore/client_metric.h"
#include "promcore/detail/builder.h"  // IWYU pragma: export
#include "promcore/detail/core_export.h"
#include "promcore/metric_type.h"

namespace promcore {

/// \brief A gauge metric to represent a value that can arbitrarily go up and
/// down.
///
/// The class represents the metric type gauge:
/// https://prometheus.io/docs/concepts/metric_types/#gauge
///
/// Gauges are typically used for measured values like temperatures or current
/// memory usage, but also "counts" that can go up and down, like the number of
/// running processes.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
template <typename TYPE>
class PROMCORE_CPP_CORE_EXPORT GaugeType {
public:

  using value_type = TYPE;

  static const MetricType metric_type{MetricType::Gauge};

  /// \brief Create a gauge that starts at 0.
  GaugeType() = default;

  /// \brief Create a gauge that starts at the given amount.
  explicit GaugeType(TYPE);

  /// \brief Increment the gauge by 1.
  void Increment();

  /// \brief Increment the gauge by the given amount.
  void Increment(TYPE);

  /// \brief Decrement the gauge by 1.
  void Decrement();

  /// \brief Decrement the gauge by the given amount.
  void Decrement(TYPE);

  /// \brief Set the gauge to the given value.
  void Set(TYPE);

  /// \brief Set the gauge to the current unix time in seconds.
  void SetToCurrentTime();

  /// \brief Get the current value of the gauge.
  TYPE Value() const;

  /// \brief Get the current value of the gauge.
  ///
  /// Collect is called by the Registry when collecting metrics.
  ClientMetric Collect() const;
private:
  void Change(TYPE);
  std::atomic<TYPE> value_{0};
};

/// \brief A gauge metric to represent a value that can arbitrarily go up and
/// down.
///
/// The class represents the metric type gauge:
/// https://prometheus.io/docs/concepts/metric_types/#gauge
///
/// Gauges are typically used for measured values like temperatures or current
/// memory usage, but also "counts" that can go up and down, like the number of
/// running processes.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class PROMCORE_CPP_CORE_EXPORT GaugeDouble : public GaugeType<double> {
 public:

  /// \brief Create a gauge that starts at 0.
  GaugeDouble() = default;

  /// \brief Create a gauge that starts at the given amount.
  explicit GaugeDouble(double);

};

/// \brief A gauge metric to represent a value that can arbitrarily go up and
/// down.
///
/// The class represents the metric type gauge:
/// https://prometheus.io/docs/concepts/metric_types/#gauge
///
/// Gauges are typically used for measured values like temperatures or current
/// memory usage, but also "counts" that can go up and down, like the number of
/// running processes.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class PROMCORE_CPP_CORE_EXPORT GaugeInt : public GaugeType<int64_t> {
public:

  /// \brief Create a gauge that starts at 0.
  GaugeInt() = default;

  /// \brief Create a gauge that starts at the given amount.
  explicit GaugeInt(int64_t);

};

/// \brief Return a builder to configure and register a Gauge metric.
///
/// @copydetails Family<>::Family()
///
/// Example usage:
///
/// \code
/// auto registry = std::make_shared<Registry>();
/// auto& gauge_family = promcore::BuildGauge()
///                          .Name("some_name")
///                          .Help("Additional description.")
///                          .Labels({{"key", "value"}})
///                          .Register(*registry);
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
/// To finish the configuration of the Gauge metric register it with
/// Register(Registry&).
PROMCORE_CPP_CORE_EXPORT detail::Builder<GaugeDouble> BuildGaugeDouble();
PROMCORE_CPP_CORE_EXPORT detail::Builder<GaugeInt> BuildGaugeInt();

}  // namespace promcore
