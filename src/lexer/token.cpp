#include <unordered_map>
#include <string_view>

#include "lexer/token.hpp"
#include "lexer/lexer.hpp"

// Using `constexpr` to define the keywords because they are known at compile time
// This will allow the compiler to optimize the code better
constexpr std::pair<std::string_view, std::pair<Token::Type, Lexer::Context>> keyword_array[] = {
	// * A - Z
	{"Else",    {Token::Type::T_ELSE,             Lexer::Context::C_DEFAULT}},
	{"If",      {Token::Type::T_IF,               Lexer::Context::C_DEFAULT}},
	//{"Integer", {Token::Type::T_INTEGER,          Lexer::Context::C_DEFAULT}},
	{"Iterate", {Token::Type::T_ITERATE,          Lexer::Context::C_DEFAULT}},
	{"Prod",    {Token::Type::T_PROD,             Lexer::Context::C_DEFAULT}},
	//{"Real",    {Token::Type::T_REAL,             Lexer::Context::C_DEFAULT}},
	{"Sum",     {Token::Type::T_SUM,              Lexer::Context::C_DEFAULT}},

	// * a - z
	{"class",    {Token::Type::T_CLASS,           Lexer::Context::C_DEFAULT}},
	{"contains", {Token::Type::T_OPERATOR_SYMBOL, Lexer::Context::C_DEFAULT}},
	{"define",   {Token::Type::T_DEFINE,          Lexer::Context::C_DEFAULT}},
	{"in",       {Token::Type::T_IN,              Lexer::Context::C_DEFAULT}},
	{"init",     {Token::Type::T_INIT,            Lexer::Context::C_CLASS}},
	{"let",      {Token::Type::T_LET,             Lexer::Context::C_DEFAULT}},
	{"none",     {Token::Type::T_NONE,            Lexer::Context::C_DEFAULT}},
	{"operator", {Token::Type::T_OPERATOR,        Lexer::Context::C_DEFINE}},
	//{"print",    {Token::Type::T_OPERATOR_SYMBOL, Lexer::Context::C_DEFAULT}},
	{"return",   {Token::Type::T_RETURN,          Lexer::Context::C_DEFAULT}},
	{"use",      {Token::Type::T_USE,             Lexer::Context::C_DEFAULT}},
};
const std::unordered_map<std::string_view, std::pair<Token::Type, Lexer::Context>> keywords(
	keyword_array, keyword_array + sizeof(keyword_array) / sizeof(keyword_array[0])
);

std::pair<Token::Type, Lexer::Context> get_work_tk_type(std::string_view lexeme)
{
	if (lexeme.empty())
	{
		return {Token::Type::T_ERROR, Lexer::Context::C_DEFAULT};
	}

	auto it = keywords.find(lexeme);
	if (it != keywords.end())
	{
		return it->second;
	}

	return {Token::Type::T_IDENTIFIER, Lexer::Context::C_DEFAULT};
}