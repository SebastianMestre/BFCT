#pragma once

#include <string>

#include "Op.hpp"
#include "ast.hpp"

AST* compile(std::string const& code);
