#include <unordered_map>
#include <string_view>

#include "lexer/token.hpp"

std::unordered_map<std::string_view, Token::Type> keywords = {
	// * A - Z
	{"Else", Token::Type::T_ELSE},
	{"If", Token::Type::T_IF},
	{"Integer", Token::Type::T_INTEGER},
	{"Iterate", Token::Type::T_ITERATE},
	{"Prod", Token::Type::T_PROD},
	{"Real", Token::Type::T_REAL},
	{"Sum", Token::Type::T_SUM},

	// * a - z
	{"class", Token::Type::T_CLASS},
	{"contains", Token::Type::T_OPERATOR_SYMBOL},
	{"define", Token::Type::T_DEFINE},
	{"in", Token::Type::T_IN},
	{"let", Token::Type::T_LET},
	{"none", Token::Type::T_NONE},
	{"operator", Token::Type::T_OPERATOR},
	{"print", Token::Type::T_OPERATOR_SYMBOL},
	{"return", Token::Type::T_RETURN},
	{"use", Token::Type::T_USE},
};

Token::Type check_work_tk_type(std::string_view lexeme)
{
	if (lexeme.empty())
	{
		return Token::Type::T_ERROR;
	}

	auto it = keywords.find(lexeme);
	if (it != keywords.end())
	{
		return it->second;
	}

	return Token::Type::T_IDENTIFIER;
}