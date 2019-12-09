#pragma once

#include <vector>

#include <cstdio>
#include <cstdint>

#include "Op.hpp"

enum class ExpressionType {
	BLOCK_EXPR,
	LOOP_EXPR,
	OP_EXPR,
	FLAT_BLOCK_EXPR
};

namespace ASTFlags {
	constexpr uint64_t none = 0x0;
	constexpr uint64_t is_trivial = 0x1;
};

// @@ Allocating an entire AST node for an Op seems wasteful.
// Could we have flag to indicate that we will store ops
// on a flat vector?
// Or maybe have body be a vector<variant<Op, AST*>> or similar?
struct AST {
	ExpressionType expr_type;

	// Op
	Op op;

	// Block or Loop
	std::vector<AST*> body;

	uint64_t flags;
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

AST* make_op_ast (Op op);

AST* make_loop_ast ();
