#pragma once

#include <string>
#include <vector>

#include "promcore/client_metric.h"
#include "promcore/detail/core_export.h"
#include "promcore/metric_type.h"

namespace promcore {

struct PROMCORE_CPP_CORE_EXPORT MetricFamily {
  std::string name;
  std::string help;
  MetricType type = MetricType::Untyped;
  std::vector<ClientMetric> metric;
};
}  // namespace promcore
