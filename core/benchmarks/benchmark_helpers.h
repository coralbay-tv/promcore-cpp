#pragma once

#include <cstddef>
#include <string>

#include "promcore/labels.h"

std::string GenerateRandomString(std::size_t length);
promcore::Labels GenerateRandomLabels(std::size_t number_of_labels);
