#pragma once

#include <vector>

#include "Op.hpp"

std::vector<Op> collapse_consecutive(std::vector<Op> const& code);

std::vector<Op> constant_folding(std::vector<Op> const& code);

std::vector<Op> optimize(std::vector<Op> const& code);
