#include <string>
#include <unordered_map>

#include "token.h"

std::unordered_map<std::string_view, TokenType> keywords = {
	// * A - Z
	{"Else", TokenType::T_ELSE},
	{"If", TokenType::T_IF},
	{"Integer", TokenType::T_INTEGER},
	{"Iterate", TokenType::T_ITERATE},
	{"Prod", TokenType::T_PROD},
	{"Real", TokenType::T_REAL},
	{"Sum", TokenType::T_SUM},

	// * a - z
	{"class", TokenType::T_CLASS},
	{"contains", TokenType::T_OPERATOR_SYM},
	{"define", TokenType::T_DEFINE},
	{"in", TokenType::T_IN},
	{"let", TokenType::T_LET},
	{"none", TokenType::T_NONE},
	{"operator", TokenType::T_OPERATOR},
	{"print", TokenType::T_OPERATOR_SYM},
	{"use", TokenType::T_USE},
};

TokenType check_word_t_type(std::string_view lexeme)
{
	if (lexeme.length() == 0)
		return TokenType::T_ERROR;
		
	auto i = keywords.find(lexeme);
	return i != keywords.end() ?
		i->second :
		TokenType::T_IDENTIFIER;
}

bool Token::is_literal(void)
{ return _type_ == TokenType::T_INTEGER_LITERAL ||  _type_ == TokenType::T_REAL_LITERAL; }

bool Token::is_identifier(void)
{ return _type_ == TokenType::T_IDENTIFIER; }

bool Token::is_operator(void)
{ return _type_ == TokenType::T_OPERATOR_SYM; }

bool Token::is_eof(void)
{ return _type_ == TokenType::T_EOF; }