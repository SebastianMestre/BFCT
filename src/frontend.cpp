#include "frontend.hpp"

AST* compile(std::string const& code)
{
    std::vector<Token> result;

	int line = 0;
	int pos = 0;

    for (char c : code)
    {
        switch (c)
        {
            case '+': result.push_back({ TokenType::Plus  , line, pos}); break;
            case '-': result.push_back({ TokenType::Minus , line, pos}); break;
            case '>': result.push_back({ TokenType::Gt    , line, pos}); break;
            case '<': result.push_back({ TokenType::Lt    , line, pos}); break;
            case '[': result.push_back({ TokenType::OBrace, line, pos}); break;
            case ']': result.push_back({ TokenType::CBrace, line, pos}); break;
            case '.': result.push_back({ TokenType::Dot   , line, pos}); break;
            case ',': result.push_back({ TokenType::Comma , line, pos}); break;
        }

		pos++;

		if(c == '\n'){
			line++;
			pos = 0;
		}
    }

    return parse_program(result, 0, result.size()).ast;
}
