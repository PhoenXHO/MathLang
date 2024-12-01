#ifndef LEXER_HPP
#define LEXER_HPP

#include <memory>
#include <string_view>

#include "lexer/token.hpp"

class Lexer
{
public:
	// The context of the lexer, which will allow the user to use certain keywords as identifiers outside of certain contexts
	// (e.g. 'init')
	enum class Context
	{
		DEFAULT,
		CLASS,
	};

private:
	std::string_view source;
	size_t pos = 0;
	size_t line = 1, column = 1;
	Context context = Context::DEFAULT;

	/// @brief Skip whitespaces and tabs
	/// @return true if a newline character is found, false otherwise
	bool skip_whitespaces();
	void skip_comment();

	char peek() const
	{ return peek(0); }
	char peek(size_t offset) const
	{ return (0 <= pos + offset && pos + offset < source.size()) ? source[pos + offset] : '\0'; }
	char peek_next() const
	{ return peek(1); }
	char advance()
	{ return advance(1); }
	char advance(size_t offset);
	void retreat()
	{ retreat(1); }
	void retreat(size_t offset)
	{ pos -= offset; }

	bool at_eol() const
	{ return source[pos] == '\n'; }
	bool at_eof() const
	{ return pos >= source.size() || source[pos] == '\0'; }

	std::unique_ptr<Token> make_tk(Token::Type type, std::string_view lexeme, size_t start_column, size_t start_position);
	std::unique_ptr<Token> make_tk(Token::Type type, std::string_view lexeme)
	{ return make_tk(type, lexeme, column, pos); }
	std::unique_ptr<Token> make_number_tk(void); // Integer/Real number
	std::unique_ptr<Token> make_operator_tk(void); // Operator symbol

public:
	Lexer() = default;
	~Lexer() = default;
	
	void reset(void)
	{
		pos = 0;
		line = 1;
		column = 1;
		source = "";
	}

	void set_source(std::string_view source)
	{ this->source = source; }
	void set_context(Context context)
	{ this->context = context; }

	// A function to scan a single token from the source code
	std::unique_ptr<Token> scan_tk(void);

	//! A temporary function to lex the entire source code
	void lex_source(std::string_view source);
};

#endif // LEXER_HPP