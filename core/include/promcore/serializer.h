#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "promcore/detail/core_export.h"
#include "promcore/metric_family.h"

namespace promcore {

class PROMCORE_CPP_CORE_EXPORT Serializer {
 public:
  virtual ~Serializer() = default;
  virtual std::string Serialize(const std::vector<MetricFamily>&) const;
  virtual void Serialize(std::ostream& out,
                         const std::vector<MetricFamily>& metrics) const = 0;
};

}  // namespace promcore
