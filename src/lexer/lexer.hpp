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
		C_DEFAULT, // General context
		C_DEFINE, // Function definition, operator definition, etc.
		C_CLASS, // Inside a class definition
	};

private:
	std::string_view source;
	size_t pos = 0;
	size_t line = 1, column = 1;
	Context context = Context::C_DEFAULT;

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
	std::unique_ptr<Token> make_word_tk(void); // Identifier/Keyword
	std::unique_ptr<Token> make_operator_tk(void); // Operator symbol

	/// @brief Check if the current context is the same as the given context. Keywords that have a default context
	/// will always be considered as keywords. Otherwise, if the context is not the same as the given context, the
	/// keyword will be considered as an identifier.
	/// @param context The context to check against
	/// @return true if the current context is the same as the given context, false otherwise
	bool is_in_context(Context context) const
	{ return this->context == context || context == Context::C_DEFAULT; }

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
};

std::pair<Token::Type, Lexer::Context> get_work_tk_type(std::string_view lexeme);

#endif // LEXER_HPP