#ifndef LEXER_H
#define LEXER_H

#include <string_view>
#include <vector>
#include <memory>

#include "token.h"

class Lexer
{
private:
	std::string_view source; // source code
	size_t pos; // current position inside `source`
	size_t line, column;

	void skip_whites(void);
	void skip_comment(void);

	char advance(void);
	char advance(int jump);
	char peek(void);
	char peek(int lookahead);
	char peek_next(void);

	std::unique_ptr<Token> make_tk(TokenType type, std::string_view lexeme, size_t start_column, size_t start_pos);
	std::unique_ptr<Token> make_number_tk(void); // Integer/real token
	std::unique_ptr<Token> make_word_tk(void); // Identifier/keyword token
	std::unique_ptr<Token> make_operator_tk(void); // Operator token

	bool at_end(void);

public:
	Lexer(std::string_view source) :
		source(source),
		pos(0),
		line(1),
		column(1)
	{}
	std::string_view get_source(void) { return source; };

	std::unique_ptr<Token> scan_tk(void);
};

#endif // LEXER_H