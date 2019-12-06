#include "ast.hpp"

void parse_error (char const* fstr, int line, int pos) {
	fprintf(stderr, fstr, line, pos);
	exit(1);
}

AST* make_op_ast (Op op) {
	return new AST {
		ExpressionType::OP_EXPR,
		op,
		{},
		{}
	};
}

AST* make_loop_ast () {
	return new AST {
		ExpressionType::LOOP_EXPR,
		{},
		{},
		{}
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

ParseResult parse_loop (std::vector<Token> const& arr, int l, int r) {

	if (arr[l].type != TokenType::OBrace)
		return { nullptr, l, r };

	AST* ast = make_loop_ast();

	// @@ Cleanup: Almost duplicated code in parse_program
	for(int i = l+1; i < r;){
		if(arr[i].type == TokenType::CBrace){
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

	// @@ Memory: we are leaking the discarded ast here.

	// @@ Print some context??
	parse_error("Unclosed loop starting at %d:%d\n", arr[l].line+1, arr[l].pos+1);
}

ParseResult parse_program (std::vector<Token> const& arr, int l, int r) {
	// @@ Cleanup: Almost duplicated code in parse_loop
	AST* ast = new AST{ExpressionType::BLOCK_EXPR, {}, {}, {}};

	for(int i = l; i < r;){
		if(arr[i].type == TokenType::CBrace){
			// @@ Print context as well?
			parse_error("Unmatched loop close at %d:%d\n", arr[i].line+1, arr[i].pos+1);
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
