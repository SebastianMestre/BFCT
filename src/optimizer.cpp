#include "optimizer.hpp"

#include <algorithm>
#include <utility>
#include <map>
#include <vector>

#include "Op.hpp"

void trivial_loop_to_mult_impl (AST* code)
{
	std::cout << "simplify_ops\n";

	// This optimization only makes sense on loops
	if(code->expr_type != ExpressionType::LOOP_EXPR){
		return;
	}

	int shift = 0;
	for (AST* ast : code->body) {
		if (ast->op.opcode == Opcode::Jmp) {
			shift += ast->op.arg1;
		}
	}

	// We end with an offset in our memory pointer.
	// Therefore, we cannot optimize
	if (shift != 0) {
		return;
	}


	{
		int pos = 0;
		std::map<int, int> deltas;
		for (AST* ast : code->body)
		{
			if (ast->op.opcode == Opcode::Jmp)
				pos += ast->op.arg1;
			else if (ast->op.opcode == Opcode::Addi)
				deltas[pos] += ast->op.arg1;
			else
				// This optimization can only be done to "trivial" loops
				// i.e. non-nested loops with a constant counter increment
				// @@ IO can be done. I need to think a bit more about it
				return;
		}

		// Counter decrement is not 1. can't optimize.
		// @@ This is not really true. You can still do this.
		// I just havent worked out all the details but you have to do some
		// modular arithmetic at runtime
		if (deltas[0] != -1)
			return;

		// This vector is the order in which we are going to do operations.
		// first = position
		// second = difference
		{
			// Thinking about Ops directly is easier than dealing with AST,
			// so we will put our Ops in a vector then convert to AST
			std::vector<Op> target;
			std::vector<std::pair<int, int>> pairs;
			// Start by jumping to offset zero, with no difference
			pairs.push_back({ 0, 0 });
			// Then to every other offset in increasing order.
			for (auto kv : deltas)
				if (kv.first != 0)
					pairs.push_back(kv);

			// Now we jump from every position to the next.
			// M-ADDing as we go
			std::adjacent_find(
			    pairs.begin(), pairs.end(), [&](auto const& lhs, auto const& rhs) {
				    // Distance between positions
				    int diff = rhs.first - lhs.first;

				    target.push_back(Op{ Opcode::Jmp, int16_t(diff) });
				    target.push_back(
				        Op{ Opcode::Madd, int16_t(rhs.second), int16_t(-rhs.first) });

					// I am using adjacent_find as pair-wise foreach, hence the
					// false predicate
				    return false;
			    });

			// Jump back to offset 0:
			target.push_back(Op{ Opcode::Jmp, int16_t(-pairs.back().first) });

			// Set it to zero
			target.push_back(Op{ Opcode::Seti, 0 });

			// Now for the conversion to AST:

			// @@ Speed. We could reuse the nodes in the original code->body
			// but doing it right now feels like premature optimization
			std::vector<AST*> result_body;
			for (Op const& op : target) {
				result_body.push_back(make_op_ast(op));
			}
			
			// @@ Memory. We are leaking the nodes in the old code->body
			code->body = std::move(result_body);
			code->expr_type = ExpressionType::BLOCK_EXPR;

			std::cout << "simplify_ops done\n";
		}
	}
}

void trivial_loop_to_mult (AST* code)
{
	bool has_loop_child = false;
	for (AST* ast : code->body) {
		if (ast->expr_type == ExpressionType::LOOP_EXPR) {
			trivial_loop_to_mult(ast);
			has_loop_child = true;
		}
	}

	if (not has_loop_child) {
		trivial_loop_to_mult_impl(code);
	}
}

void optimize(AST* code)
{
    trivial_loop_to_mult(code);
}

