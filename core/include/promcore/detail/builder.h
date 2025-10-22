#pragma once

#include <string>

#include "promcore/labels.h"

// IWYU pragma: private
// IWYU pragma: no_include "prometheus/family.h"

namespace promcore {

template <typename T>
class Family;    // IWYU pragma: keep
class Registry;  // IWYU pragma: keep

namespace detail {

template <typename T>
class Builder {
 public:
  Builder& Labels(const ::promcore::Labels& labels);
  Builder& Name(const std::string&);
  Builder& Help(const std::string&);
  Family<T>& Register(Registry&);

 private:
  ::promcore::Labels labels_;
  std::string name_;
  std::string help_;
};

}  // namespace detail
}  // namespace promcore
