#include "promcore/serializer.h"

#include <sstream>  // IWYU pragma: keep

namespace promcore {

std::string Serializer::Serialize(
    const std::vector<MetricFamily>& metrics) const {
  std::ostringstream ss;
  Serialize(ss, metrics);
  return ss.str();
}
}  // namespace promcore
