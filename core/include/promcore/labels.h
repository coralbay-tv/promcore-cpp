#pragma once

#include <map>
#include <string>

namespace promcore {

/// \brief Multiple labels and their value.
using Labels = std::map<std::string, std::string>;

/// \brief Single label and its value.
using Label = Labels::value_type;

}  // namespace promcore
