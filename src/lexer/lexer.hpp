#ifndef LEXER_HPP
#define LEXER_HPP

#include <memory>
#include <string_view>

#include "lexer/token.hpp"
#include "util/globals.hpp" // for `globals::source`

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
	SourceLocation location;
	Context context = Context::C_DEFAULT;

	/// @brief Skip whitespaces and comments
	/// @return true if a newline character is found, false otherwise
	bool skip_whitespaces(void);
	void skip_comment(void);

	char peek(void) const
	{ return peek(0); }
	char peek(size_t offset) const
	{ return (0 <= location.position + offset && location.position + offset < source.size()) ?
		source[location.position + offset] : '\0'; }
	char peek_next(void) const
	{ return peek(1); }
	char advance(void)
	{ return advance(1); }
	char advance(size_t offset);
	void retreat(void)
	{ retreat(1); }
	void retreat(size_t offset)
	{ location.position -= offset; }

	bool at_eol(void) const
	{ return source[location.position] == '\n'; }
	bool at_eof(void) const
	{ return location.position >= source.size() || source[location.position] == '\0'; }

	std::shared_ptr<Token> make_tk(Token::Type type, std::string_view lexeme, SourceLocation location);
	std::shared_ptr<Token> make_tk(Token::Type type, std::string_view lexeme)
	{ return make_tk(type, lexeme, location); }
	std::shared_ptr<Token> make_number_tk(void); // Integer/Real number
	std::shared_ptr<Token> make_word_tk(void); // Identifier/Keyword
	std::shared_ptr<Token> make_operator_tk(void); // Operator symbol

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
		location = SourceLocation();
		source = "";
	}

	void update_source(void)
	{ this->source = globals::source; }
	void set_context(Context context)
	{ this->context = context; }

	/// @brief Scan the next token
	/// @return The next token
	std::shared_ptr<Token> scan_tk(void);
	/// @brief Go back to the previous token.
	/// @param prev_tk The previous token
	void retreat_tk(std::shared_ptr<Token> & prev_tk);
};

std::pair<Token::Type, Lexer::Context> get_work_tk_type(std::string_view lexeme);

#endif // LEXER_HPP