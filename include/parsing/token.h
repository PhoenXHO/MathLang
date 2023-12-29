#ifndef TOKEN_H
#define TOKEN_H

#include <string_view>

struct Token
{
	enum class TokenType {
		// Keywords
		T_LET, T_DEFINE, T_CLASS,
		T_IN,
		T_NONE,
		T_IF, T_ELSE,
		T_ITERATE,
		T_SUM, T_PROD,
		T_USE, T_OPERATOR,
		
		// Operators
		T_OPERATOR_SYM,

		// Types
		T_MATHOBJ,
		T_NATURAL, T_INTEGER, T_RATIONAL, T_REAL, T_COMPLEX,
		T_BOOL,
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
		T_LEFT_SQR_BR, T_RIGHT_SQR_BR,
		T_LEFT_CURL_BR, T_RIGHT_CURL_BR,
		T_RETURN,

		// Others
		T_ERROR, T_EOF
	};

	Token(TokenType type, std::string_view lexeme, size_t line, size_t column, size_t position) :
		_type_(type),
		_lexeme_(lexeme),
		_line_(line),
		_column_(column),
		_pos_(position)
	{}

	TokenType type(void) { return _type_; }
	std::string_view lexeme(void) { return _lexeme_; }
	size_t line(void) { return _line_; }
	size_t column(void) { return _column_; }
	size_t position(void) { return _pos_; }

	bool is_literal(void);
	bool is_identifier(void);
	bool is_operator(void);
	bool is_eof(void);

private:
	TokenType _type_;
	std::string_view _lexeme_;
	size_t _line_, _column_;
	size_t _pos_;
};
typedef Token::TokenType TokenType;

struct EOFToken : Token
{
	EOFToken(size_t line, size_t column, size_t position) :
		Token (
			TokenType::T_EOF,
			"",
			line,
			column,
			position
		)
	{}
};

std::string tk_type_to_string(TokenType t_type);
TokenType check_word_t_type(std::string_view lexeme);

#endif // TOKEN_H