#pragma once

#include <vector>

#include <cstdio>
#include <cstdint>

#include "Op.hpp"

enum class ExpressionType {
	BLOCK_EXPR,
	LOOP_EXPR,
	OP_EXPR
};

struct AST {
	ExpressionType expr_type;

	// Op
	Op op;

	// Loop
	std::vector<AST*> loop_body;

	// Block
	std::vector<AST*> block_body;
};

struct ParseResult {
	AST* ast;
	int l;
	int r;
};

void parse_error (char const* fstr, int i);

ParseResult parse_op (std::vector<Token> const& arr, int l, int r);

ParseResult parse_loop (std::vector<Token> const& arr, int l, int r);

ParseResult parse_program (std::vector<Token> const& arr, int l, int r);

