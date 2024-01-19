#include "lexer.h"
#include "error.h"
#include "debug.h"
#include "vm.h"
#include "token.h"
#include "globals.h"

#include <iostream>

std::shared_ptr<Lexer> lexer;

bool is_operator_sym(char c);

std::unique_ptr<Token> Lexer::scan_tk(void)
{
	std::unique_ptr<Token> token;

	skip_whites(); // skips comments too
	char curr { peek() }, next { peek_next() };

	// * NUMBERS
	/* numbers follow this format:
		d [d*] [. [d*]] // 1, 1.23, 12.
		or
		[d d*] . d [d*] // .5, .54, 1.2
		`d` is a digit
	*/
	if (isdigit(curr) || (curr == '.' && isdigit(next)))
	{
		token = make_number_tk();
		return token;
	}

	// * IDENTIFIERS & KEYWORDS
	if (isalpha(curr) || curr == '_')
	{
		token = make_word_tk();
		return token;
	}

	// * OPERATORS
	if (is_operator_sym(curr))
	{
		if (curr == ':' && next == '-' && peek(2) == '>' && !is_operator_sym(peek(3)))
		{
			token = make_tk(TokenType::T_COLON_ARROW, ":->", column, pos);
			advance(3);
		}
		else if (curr == ':')
		{
			if (next == '=' && !is_operator_sym(peek(2)))
			{
				token = make_tk(TokenType::T_COLON_EQUAL, ":=", column, pos);
				advance(2);
			}
			else if (!is_operator_sym(next))
			{
				token = make_tk(TokenType::T_COLON, ":", column, pos);
				advance();
			}
		}
		else if (curr == '-' && next == '>' && !is_operator_sym(peek(2)))
		{
			token = make_tk(TokenType::T_ARROW, "->", column, pos);
			advance(2);
		}
		else
			token = make_operator_tk();
		return token;
	}
	
	switch (curr)
	{
		// * SPECIAL SYMBOLS
		case ',':
			token = make_tk(TokenType::T_COMMA, ",", column, pos);
			break;
		case ';':
			token = make_tk(TokenType::T_SEMICOLON, ";", column, pos);
			break;
		case '(':
			token = make_tk(TokenType::T_LEFT_PAREN, "(", column, pos);
			break;
		case ')':
			token = make_tk(TokenType::T_RIGHT_PAREN, ")", column, pos);
			break;
		case '[':
			token = make_tk(TokenType::T_LEFT_BRACKET, "[", column, pos);
			break;
		case ']':
			token = make_tk(TokenType::T_RIGHT_BRACKET, "]", column, pos);
			break;
		case '{':
			token = make_tk(TokenType::T_LEFT_BRACE, "{", column, pos);
			break;
		case '}':
			token = make_tk(TokenType::T_RIGHT_BRACE, "}", column, pos);
			break;

		// * OTHERS
		case '\0':
			token = std::make_unique<EOFToken>(line, column, pos);
			break;
		
		default:
			token = make_tk(TokenType::T_ERROR, "", column, pos);
			break;
	}

	advance();

	return token;
}

std::unique_ptr<Token> Lexer::make_operator_tk(void)
{
	size_t lexeme_length = 1; 
	size_t start_pos = pos, start_col = column;

	advance();
	char c = peek();
	while (!at_end() && is_operator_sym(c) && lexeme_length <= 3)
	{
		advance();
		c = peek();
		lexeme_length++;
	}
	
	std::string_view lexeme = source.substr(start_pos, lexeme_length);
	return make_tk(TokenType::T_OPERATOR_SYM, lexeme, start_col, start_pos);
}

std::unique_ptr<Token> Lexer::make_word_tk(void)
{
	size_t lexeme_length = 1; 
	size_t start_pos = pos, start_col = column;

	advance();
	char c = peek();
	while (!at_end() && (isalnum(c) || c == '_'))
	{
		advance();
		c = peek();
		lexeme_length++;
	}
	
	std::string_view lexeme = source.substr(start_pos, lexeme_length);
	TokenType t_type = check_word_t_type(lexeme);

	return make_tk(t_type, lexeme, start_col, start_pos);
}

std::unique_ptr<Token> Lexer::make_number_tk(void)
{
	size_t lexeme_length = 1; 
	size_t start_pos = pos, start_col = column;

	/* Because we jumped one character ahead,
	   we need to check if the character is a dot
	   or just a regular digit (the check was done in `scan_t`)
	   before we move to the next character.
	   This is to support the format: . d [d*]
	*/
	bool has_dot = (peek() == '.');
	bool is_error = false;

	advance();
	char c = peek();
	while (!at_end() && (isdigit(c) || c == '.'))
	{
		if (c == '.')
		{
			if (has_dot)
				is_error = true;

			has_dot = true;
		}

		advance();
		c = peek();
		lexeme_length++;
	}

	std::string_view lexeme = source.substr(start_pos, lexeme_length);

	if (is_error)
	{
		std::unique_ptr<Error> t_err { new LexicalError(
			lexeme,
			"too many decimal points in number",
			line,
			start_col,
			start_pos
		)};
		ErrorHandler::push_error(t_err);
	}

	return make_tk(
		is_error ?
			TokenType::T_ERROR :
			(has_dot ?
				TokenType::T_REAL_LITERAL :
				TokenType::T_INTEGER_LITERAL),
		lexeme,
		start_col,
		start_pos
	);
}

std::unique_ptr<Token> Lexer::make_tk(TokenType type, std::string_view lexeme, size_t start_column, size_t start_pos)
{
	return std::make_unique<Token>(
		type,
		lexeme,
		line,
		start_column,
		start_pos
	);
}

void Lexer::skip_whites(void)
{
	while (!at_end())
	{
		switch (peek())
		{
			case ' ': case '\r':
				advance();
				break;
			case '\t':
				advance();
				break;
			case '\n':
				line++;
				column = 0;
				advance();
				break;
			case '/':
				if (peek_next() == '/')
					skip_comment();
				else
					return;
				break;

			default: return;
		}
	}
}

void Lexer::skip_comment(void)
{
	advance();
	while (!at_end() && peek() != '\n')
		advance();
}

bool is_operator_sym(char c)
{
	return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' ||
			c == '=' || c == '<' || c == '>' || c == '|' || c == '!' ||
			c == '@' || c == '#' || c == '$' || c == '%' || c == '&' ||
			c == '~' || c == '?' || c == ':');
}

char Lexer::peek(void)
{ return peek(0); }

char Lexer::peek(int lookahead)
{
	return (0 <= pos + lookahead && pos + lookahead < source.length()) ?
		source[pos + lookahead] :
		'\0';
}

char Lexer::peek_next(void)
{ return peek(1); }

char Lexer::advance(void)
{ return advance(1); }

char Lexer::advance(int jump)
{
	if (source.length() < pos + jump )
		return '\0';
	pos += jump; column += jump;
	return source[pos - jump];
}

bool Lexer::at_end(void)
{ return pos >= source.length() || source[pos] == '\0'; }