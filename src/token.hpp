#pragma once

enum class TokenType
{
	OBrace,
	CBrace,
	Plus,
	Minus,
	Comma,
	Dot,
	Lt,
	Gt
};

struct Token
{
	TokenType type;
	int line;
	int pos;
};
