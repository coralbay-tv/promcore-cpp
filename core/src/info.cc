#include "promcore/info.h"

namespace promcore {

ClientMetric Info::Collect() const {
  ClientMetric metric;
  metric.info.value = 1;
  return metric;
}

}  // namespace promcore
