#pragma once

#include <string>

#include "promcore/detail/core_export.h"
#include "promcore/metric_type.h"

namespace promcore {

PROMCORE_CPP_CORE_EXPORT bool CheckMetricName(const std::string& name);
PROMCORE_CPP_CORE_EXPORT bool CheckLabelName(const std::string& name,
                                               MetricType type);
}  // namespace promcore
