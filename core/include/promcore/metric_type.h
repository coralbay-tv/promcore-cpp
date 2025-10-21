#pragma once

namespace promcore {

enum class MetricType {
  Counter,
  Gauge,
  Summary,
  Untyped,
  Histogram,
  Info,
};

}  // namespace promcore
