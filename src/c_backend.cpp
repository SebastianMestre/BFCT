#include "c_backend.hpp"

#include <iostream>
#include <sstream>

namespace {
constexpr char const* output_header = "#include <stdio.h>\n"
                                      "#include <stdint.h>\n"
                                      "#define BUFFER_SIZE 1000\n"
                                      "int main(){\n"
                                      "uint8_t buffer[BUFFER_SIZE] = {};\n"
                                      "uint8_t* p = buffer + BUFFER_SIZE / 2;\n";

constexpr char const* output_footer = "return 0;\n"
                                      "}\n";
}

std::string write_c_code(AST* code) ;

std::string write_c_ast(AST* code) ;

std::string write_c_block (std::vector<AST*> body) ;
std::string write_c_flat_block (std::vector<AST*> body) ;

std::string write_c_loop (std::vector<AST*> body) ;

std::string write_c_op (Op op) ;

std::string write_c_code(AST* code) {
    std::stringstream result;
    result << output_header;

	result << write_c_ast(code);

    result << output_footer;

    return result.str();
}

std::string write_c_ast(AST* code) {
    std::stringstream result;

	// switch(code->expr_type){
	// 	case ExpressionType::BLOCK_EXPR:
	// 		std::cout << "codegen block\n";
	// 		break;
	// 	case ExpressionType::LOOP_EXPR:
	// 		std::cout << "codegen loop\n";
	// 		break;
	// 	case ExpressionType::OP_EXPR:
	// 		std::cout << "codegen op\n";
	// 		break;
	// }

	switch(code->expr_type){
		case ExpressionType::FLAT_BLOCK_EXPR:
			return write_c_flat_block(code->body);
		case ExpressionType::BLOCK_EXPR:
			return write_c_block(code->body);
		case ExpressionType::LOOP_EXPR:
			return write_c_loop(code->body);
		case ExpressionType::OP_EXPR:
			return write_c_op(code->op);
	}
    return result.str();
}

std::string write_c_flat_block (std::vector<AST*> body) {
	std::stringstream result;

    for (AST* ast : body)
    {
		result << write_c_ast(ast);
    }

	return result.str();
}

std::string write_c_block (std::vector<AST*> body) {
	std::stringstream result;

    for (AST* ast : body)
    {
		result << write_c_ast(ast);
    }

	return result.str();
}

std::string write_c_loop (std::vector<AST*> body) {
	std::stringstream result;

	result << "while(*p){\n";
    for (AST* ast : body)
    {
		result << write_c_ast(ast);
    }
	result << "}\n";

	return result.str();
}

std::string write_c_op (Op op) {
    std::stringstream result;

	switch (op.opcode)
	{
		/*
		case Opcode::Lop: result << "while(*p){\n"; break;
		case Opcode::Lcl: result << "}\n"; break;

		case Opcode::Cop: result << "if(*p){\n"; break;
		case Opcode::Ccl: result << "}\n"; break;
		*/

		case Opcode::Put: result << "putchar(*p);\n"; break;
		case Opcode::Get: result << "*p = getchar();\n"; break;

		case Opcode::Addi: result << "*p += " << int(op.arg1) << ";\n"; break;
		case Opcode::Seti: result << "*p = "  << int(op.arg1) << ";\n"; break;
		case Opcode::Muli: result << "*p *= " << int(op.arg1) << ";\n"; break;

		case Opcode::Madd: result << "*p += p[" << int(op.arg2) << "] * " << int(op.arg1) << ";\n"; break;

		case Opcode::Jmp: result << "p += " << int(op.arg1) << ";\n"; break;

		default: std::cerr << "Bad opcode: " << int(op.opcode) << '\n';
	}

	return result.str();
}
