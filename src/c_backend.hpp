#pragma once

#include <string>
#include <vector>

#include "Op.hpp"

std::string write_c_code(std::vector<Op> const& code);
