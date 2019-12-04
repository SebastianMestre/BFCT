#include "ast.hpp"

void parse_error (char const* fstr, int i) {
	fprintf(stderr, fstr, i);
	exit(1);
}

ParseResult parse_op (std::vector<Op> const& arr, int l, int r) {
	if(
		arr[l].opcode == Opcode::Addi ||
		arr[l].opcode == Opcode::Jmp ||
		arr[l].opcode == Opcode::Put ||
		arr[l].opcode == Opcode::Get
	){
		return {new AST {ExpressionType::OP_EXPR, arr[l], {}, {}}, l+1, r};
	} else {
		return { nullptr, l, r };
	}
}

ParseResult parse_loop (std::vector<Op> const& arr, int l, int r) {
	std::cout << "Parsing loop\n";
	if (arr[l].opcode != Opcode::Lop)
		return { nullptr, l, r };

	AST* ast = new AST{ExpressionType::LOOP_EXPR, {}, {}, {}};

	// @@ Cleanup: Almost duplicated code in parse_program
	for(int i = l+1; i < r;){
		if(arr[i].opcode == Opcode::Lcl){
			return {ast, i+1, r};
		}

		ParseResult try_op = parse_op(arr, i, r);
		if(try_op.ast){
			ast->loop_body.push_back(try_op.ast);
			i = try_op.l;
			continue;
		}

		ParseResult try_loop = parse_loop(arr, i, r);
		if(try_loop.ast){
			ast->loop_body.push_back(try_loop.ast);
			i = try_loop.l;
			continue;
		}
	}

	// @@ Cleanup: Print something better, line number, context, etc
	parse_error("Unclosed loop starting at token %d ", l);
}

ParseResult parse_program (std::vector<Op> const& arr, int l, int r) {
	// @@ Cleanup: Almost duplicated code in parse_loop
	AST* ast = new AST{ExpressionType::BLOCK_EXPR, {}, {}, {}};

	for(int i = l+1; i < r;){
		if(arr[i].opcode == Opcode::Lcl){
			// @@ Cleanup: Print line number, etc instead of token number
			parse_error("Unmatched loop close at token %d ", i);
		}

		ParseResult try_op = parse_op(arr, i, r);
		if(try_op.ast){
			ast->block_body.push_back(try_op.ast);
			i = try_op.l;
			continue;
		}

		ParseResult try_loop = parse_loop(arr, i, r);
		if(try_loop.ast){
			ast->block_body.push_back(try_loop.ast);
			i = try_loop.l;
			continue;
		}
	}

	return {ast, r, r};
}
