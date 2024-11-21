#ifndef TOKEN_H
#define TOKEN_H

#include <string_view>
#include <string>
#include <unordered_map>
#include <iostream>

struct Token
{
	enum class Type
	{
		// Keywords
		T_LET, T_DEFINE, T_CLASS,
		T_IN, T_IF, T_ELSE,
		T_ITERATE, T_SUM, T_PROD,
		T_USE, T_OPERATOR,
		T_NONE,
		
		// Operators
		T_OPERATOR_SYMBOL,

		// Types
		T_MATHOBJ,
		T_NATURAL, T_INTEGER, T_RATIONAL, T_REAL, T_COMPLEX,
		T_BOOLEAN,
		T_SET, T_LIST, T_VECTOR,
		T_STRING, T_CHAR,

		// Identifiers
		T_IDENTIFIER,

		// Literals
		T_INTEGER_LITERAL,
		T_REAL_LITERAL,
		T_I,
		T_STRING_LITERAL,
		T_CHAR_LITERAL,
		T_SET_LITERAL,
		T_LIST_LITERAL,
		T_VECTOR_LITERAL,
		T_TRUE, T_FALSE,
		
		// Special symbols
		T_COMMA, T_SEMICOLON,
		T_LEFT_PAREN, T_RIGHT_PAREN,
		T_LEFT_BRACKET, T_RIGHT_BRACKET,
		T_LEFT_BRACE, T_RIGHT_BRACE,
		T_ARROW, T_COLON, T_COLON_ARROW,
		T_COLON_EQUAL,
		T_RETURN,

		// Others
		T_ERROR, T_EOF
	};

	Token(Type type, std::string_view lexeme, size_t line, size_t column, size_t position)
		: m_type(type), m_lexeme(lexeme), m_line(line), m_column(column), m_position(position) {}

	Type type(void) const { return m_type; }
	std::string_view lexeme(void) const { return m_lexeme; }
	size_t line(void) const { return m_line; }
	size_t column(void) const { return m_column; }
	size_t position(void) const { return m_position; }

	bool is_literal(void) const
	{
		return m_type == Type::T_INTEGER_LITERAL ||
			   m_type == Type::T_REAL_LITERAL ||
			   m_type == Type::T_STRING_LITERAL ||
			   m_type == Type::T_CHAR_LITERAL ||
			   m_type == Type::T_SET_LITERAL ||
			   m_type == Type::T_LIST_LITERAL ||
			   m_type == Type::T_VECTOR_LITERAL ||
			   m_type == Type::T_TRUE ||
			   m_type == Type::T_FALSE;
	}
	bool is_identifier(void) const
	{
		return m_type == Type::T_IDENTIFIER;
	}
	bool is_operator(void) const
	{
		return m_type == Type::T_OPERATOR_SYMBOL;
	}
	bool is_eof(void) const
	{
		return m_type == Type::T_EOF;
	}

	//* Debugging
	static std::string type_to_string(Type type);
	friend std::ostream & operator<<(std::ostream & os, const Token & token);

private:
	Type m_type; // type of the token
	std::string_view m_lexeme; // the actual token
	size_t m_line, m_column; // line and column of the token in the source code
	size_t m_position; // position of the token in the source code
};

#endif // TOKEN_H