#include <string>
#include <unordered_map>

#include "token.h"

TokenType match_keyword(std::string_view keyword, std::string_view str, TokenType type);

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

TokenType match_keyword(std::string_view keyword, std::string_view str, TokenType type)
{
	return str == keyword ?
		type :
		TokenType::T_IDENTIFIER;
}

std::string tk_type_to_string(TokenType t_type)
{
	switch (t_type)
	{
		// * KEYWORDS
		case TokenType::T_IDENTIFIER:
			return "IDENTIFIER";

		// * OPERATORS
		case TokenType::T_OPERATOR_SYM:
			return "OPERATOR_SYM";

		// * LITERALS
		case TokenType::T_INTEGER_LITERAL:
			return "INT";
		case TokenType::T_REAL_LITERAL:
			return "REAL";

		// * SPECIAL SYMBOLS
		case TokenType::T_COMMA:
			return "COMMA";
		case TokenType::T_SEMICOLON:
			return "SEMICOLON";
		case TokenType::T_LEFT_PAREN:
			return "LEFT PAREN";
		case TokenType::T_RIGHT_PAREN:
			return "RIGHT PAREN";
		case TokenType::T_LEFT_SQR_BR:
			return "LEFT SQUARE BR";
		case TokenType::T_RIGHT_SQR_BR:
			return "RIGHT SQUARE BR";
		case TokenType::T_LEFT_CURL_BR:
			return "LEFT CURLY BR";
		case TokenType::T_RIGHT_CURL_BR:
			return "RIGHT CURLY BR";
		case TokenType::T_RETURN:
			return "RETURN";

		// * OTHERS
		case TokenType::T_ERROR:
			return "ERROR";
		case TokenType::T_EOF:
			return "EOF";

		default:
			return "KEYWORD";
	}
}

bool Token::is_literal(void)
{ return _type_ == TokenType::T_INTEGER_LITERAL ||  _type_ == TokenType::T_REAL_LITERAL; }

bool Token::is_identifier(void)
{ return _type_ == TokenType::T_IDENTIFIER; }

bool Token::is_operator(void)
{ return _type_ == TokenType::T_OPERATOR_SYM; }

bool Token::is_eof(void)
{ return _type_ == TokenType::T_EOF; }