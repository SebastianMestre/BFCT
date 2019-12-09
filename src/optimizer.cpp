#include "optimizer.hpp"

#include <algorithm>
#include <utility>
#include <map>
#include <vector>

#include "Op.hpp"

void trivial_loop_to_mult_impl (AST* code)
{
}

void trivial_loop_to_mult (AST* code)
{
}

// @@ Think of a better name for this optimization
void collapse_blocks_impl (AST* code) {
	assert(code->expr_type == ExpressionType::FLAT_BLOCK_EXPR);

	if(not code->flags & ASTFlags::is_trivial)
		return;

	int final_pos = 0;
	std::map<int, int> deltas;
	for (AST* ast : code->body)
	{
		if (ast->op.opcode == Opcode::Jmp)
			final_pos += ast->op.arg1;
		else if (ast->op.opcode == Opcode::Addi)
			deltas[final_pos] += ast->op.arg1;

		// This block must be trivial. therefore, there are only adds and jmps
	}

	std::vector<std::pair<int, int>> pairs;
	// Then to every other offset in increasing order.
	for (auto kv : deltas)
		pairs.push_back(kv);

	std::vector<Op> target;
	int current_pos = 0;
	for(auto kv : pairs){
		int diff = kv.first - current_pos;

		if(kv.second){
			if(diff)
				target.push_back(Op{ Opcode::Jmp, diff });

			target.push_back(Op{ Opcode::Addi, kv.second });

			current_pos = kv.first;
		}
	}

	if(final_pos - current_pos != 0)
		target.push_back(Op{ Opcode::Jmp, final_pos-current_pos });

	// Now for the conversion to AST:

	// @@ Speed. We could reuse the nodes in the original code->body
	// but doing it right now feels like premature optimization
	std::vector<AST*> result_body;
	for (Op const& op : target) {
		result_body.push_back(make_op_ast(op));
	}

	// @@ Memory. We are leaking the nodes in the old code->body
	code->body = std::move(result_body);
}

void collapse_blocks (AST* code) {
	switch (code->expr_type) {
	case ExpressionType::FLAT_BLOCK_EXPR:
		return collapse_blocks_impl(code);

	case ExpressionType::BLOCK_EXPR: /* fallthrough */
	case ExpressionType::LOOP_EXPR:
		for (AST* ast : code->body)
			collapse_blocks(ast);
		return;

	default:
		return;
	}
}

void optimize(AST* code)
{
	collapse_blocks(code);
}

