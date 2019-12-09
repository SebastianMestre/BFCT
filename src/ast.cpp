#include "ast.hpp"

#include <cassert>

void parse_error (char const* fstr, int line, int pos) {
	fprintf(stderr, fstr, line, pos);
	exit(1);
}

AST* make_op_ast (Op op) {
	return new AST {
		ExpressionType::OP_EXPR,
		op,
		{},
		ASTFlags::none
	};
}

AST* make_loop_ast () {
	return new AST {
		ExpressionType::LOOP_EXPR,
		{},
		{},
		ASTFlags::none
	};
}

AST* make_block_ast () {
	return new AST {
		ExpressionType::BLOCK_EXPR,
		{},
		{},
		ASTFlags::none
	};
}

AST* make_flat_block_ast () {
	return new AST {
		ExpressionType::FLAT_BLOCK_EXPR,
		{},
		{},
		ASTFlags::none
	};
}

ParseResult parse_op (std::vector<Token> const& arr, int l, int r) {
	switch (arr[l].type) {
		case TokenType::Plus:
			return {make_op_ast({Opcode::Addi, 1}), l+1, r};
			break;
		case TokenType::Minus:
			return {make_op_ast({Opcode::Addi, -1}), l+1, r};
			break;
		case TokenType::Lt:
			return {make_op_ast({Opcode::Jmp, -1}), l+1, r};
			break;
		case TokenType::Gt:
			return {make_op_ast({Opcode::Jmp, 1}), l+1, r};
			break;
		case TokenType::Dot:
			return {make_op_ast({Opcode::Put}), l+1, r};
			break;
		case TokenType::Comma:
			return {make_op_ast({Opcode::Get}), l+1, r};
			break;
		default:
			return {nullptr, l, r};
	}
}

ParseResult parse_simple_block (std::vector<Token> const& arr, int l, int r) {

	// We do this check here instead of in the loop to save an allocation
	if(
		arr[l].type == TokenType::CBrace ||
		arr[l].type == TokenType::OBrace
	){
		return {nullptr, l, r};
	}

	AST* ast = make_flat_block_ast();
	bool is_trivial = true;

	for(int i = l; i < r;){
		// We only want what's between and inside loops, so we return before we
		// touch loop starts or ends
		if(
			arr[i].type == TokenType::CBrace ||
			arr[i].type == TokenType::OBrace
		){
			assert(ast->body.size() > 0 &&
				"This can only not happen on the first iteration.\n"
				"Since we check before the loop, it should never happen.\n");

			if(is_trivial){
				ast->flags |= ASTFlags::is_trivial;
			}

			return {ast, i, r};
		}

		ParseResult try_op = parse_op(arr, i, r);
		if(try_op.ast){
			ast->body.push_back(try_op.ast);
			i = try_op.l;

			if (
				try_op.ast->op.opcode == Opcode::Get ||
				try_op.ast->op.opcode == Opcode::Put
			) {
				is_trivial = false;
			}

			continue;
		}
	}

	return {ast, r, r};
}

ParseResult parse_loop (std::vector<Token> const& arr, int l, int r) {

	if (arr[l].type != TokenType::OBrace)
		return { nullptr, l, r };

	AST* ast = make_loop_ast();

	// @@ Cleanup: Almost duplicated code in parse_program
	for(int i = l+1; i < r;){
		if(arr[i].type == TokenType::CBrace){
			return {ast, i+1, r};
		}

		ParseResult try_block = parse_simple_block(arr, i, r);
		if(try_block.ast){
			ast->body.push_back(try_block.ast);
			i = try_block.l;
			continue;
		}

		ParseResult try_loop = parse_loop(arr, i, r);
		if(try_loop.ast){
			ast->body.push_back(try_loop.ast);
			i = try_loop.l;
			continue;
		}
	}

	// @@ Print some context??
	parse_error("Unclosed loop starting at %d:%d\n", arr[l].line+1, arr[l].pos+1);


	// Right now failing to parse a loop terminates the compiler but maybe we'll
	// want to do something else in the future, so keep this return in here,
	// just in case

	// @@ Memory. we are leaking the discarded ast.

	return { nullptr, l, r };
}

ParseResult parse_program (std::vector<Token> const& arr, int l, int r) {
	// @@ Cleanup: Almost duplicated code in parse_loop
	AST* ast = new AST{ExpressionType::BLOCK_EXPR, {}, {}};

	for(int i = l; i < r;){
		if(arr[i].type == TokenType::CBrace){
			// @@ Print context as well?
			parse_error("Unmatched loop close at %d:%d\n", arr[i].line+1, arr[i].pos+1);
		}

		ParseResult try_block = parse_simple_block(arr, i, r);
		if(try_block.ast){
			ast->body.push_back(try_block.ast);
			i = try_block.l;
			continue;
		}

		ParseResult try_loop = parse_loop(arr, i, r);
		if(try_loop.ast){
			ast->body.push_back(try_loop.ast);
			i = try_loop.l;
			continue;
		}
	}

	return {ast, r, r};
}
