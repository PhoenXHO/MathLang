#include <iostream>

#include "lexer/lexer.hpp" // for `Lexer`
#include "global/config.hpp" // for `config::verbose`
#include "global/globals.hpp" // for `globals::error_handler`

bool is_operator_sym(char c);

std::unique_ptr<Token> Lexer::scan_tk(void)
{
	std::unique_ptr<Token> token = nullptr;

	bool new_line = skip_whitespaces(); // Skip whitespaces and comments

	// If a newline character is found, return a newline token
	if (new_line)
	{
		return make_tk(Token::Type::T_EOL, "\n");
	}

	char curr { peek() }, next { peek_next() };

	//* NUMBERS
	/* Numbers follow the format (in BNF):
		d [d*] [. d*]     # e.g 1, 1.23, 12
		or
		[d d*] . d [d*]     # e.g. .5, .54, 1.2

		where `d` is a digit
	*/
	if (isdigit(curr) || (curr == '.' && isdigit(next)))
	{
		token = make_number_tk();
		return token;
	}

	//* OPERATORS
	/* Operators are defined as any combination of the following characters (max 3 characters):
		+ - * / % ^ ! = < > & | ~ ? : @ # $
	*/
	if (is_operator_sym(curr))
	{
		if (curr == ':' && next == '-' && peek(2) == '>') // :-> (Colon Arrow)
		{
			token = make_tk(Token::Type::T_COLON_ARROW, ":->");
			advance(3);
		}
		else if (curr == ':') // : or :=
		{
			if (next == '=' && !is_operator_sym(peek(2))) // :=
			{
				token = make_tk(Token::Type::T_COLON_EQUAL, ":=");
				advance(2);
			}
			else if (!is_operator_sym(next)) // :
			{
				token = make_tk(Token::Type::T_COLON, ":");
				advance();
			}
		}
		else if (curr == '-' && next == '>' && !is_operator_sym(peek(2))) // -> (Arrow)
		{
			token = make_tk(Token::Type::T_ARROW, "->");
			advance(2);
		}
		else
		{
			token = make_operator_tk();
		}
		return token;
	}

	switch (curr)
	{
	//* SINGLE CHARACTER TOKENS
	case ',':
		token = make_tk(Token::Type::T_COMMA, ",");
		break;
	case ';':
		token = make_tk(Token::Type::T_SEMICOLON, ";");
		break;
	case '.':
		token = make_tk(Token::Type::T_DOT, ".");
		break;
	case '(':
		token = make_tk(Token::Type::T_LEFT_PAREN, "(");
		break;
	case ')':
		token = make_tk(Token::Type::T_RIGHT_PAREN, ")");
		break;
	case '[':
		token = make_tk(Token::Type::T_LEFT_BRACKET, "[");
		break;
	case ']':
		token = make_tk(Token::Type::T_RIGHT_BRACKET, "]");
		break;
	case '{':
		token = make_tk(Token::Type::T_LEFT_BRACE, "{");
		break;
	case '}':
		token = make_tk(Token::Type::T_RIGHT_BRACE, "}");
		break;

	//* OTHER TOKENS
	case '\0':
		token = make_tk(Token::Type::T_EOF, "");
		break;
	
	default:
		// Log an error for invalid characters
		globals::error_handler.log_lexical_error(
			"Invalid character '" + std::string(1, curr) + "'",
			line,
			column,
			pos,
			true
		);
		break;
	}
	
	advance();
	return token;
}

std::unique_ptr<Token> Lexer::make_number_tk(void)
{
	size_t lexeme_length = 1; // 1 because we already have the first digit (or dot)
	size_t start_column = column, start_position = pos;

	/* We need to check if the character is a dot
	   or just a regular digit (guarranteed to be a digit or a dot)
	   before we move to the next character.
	   This is to support the format: . d [d*]
	*/
	bool has_dot = peek() == '.';
	bool is_error = false;

	advance(); // Move to the next character
	char curr = peek();
	while (!at_eof() && (isdigit(curr) || curr == '.'))
	{
		if (curr == '.')
		{
			if (has_dot)
				is_error = true;
			
			has_dot = true;
		}
		lexeme_length++;
		advance();
		curr = peek();
	}

	std::string_view lexeme = source.substr(start_position, lexeme_length);
	std::string lexeme_str = std::string(lexeme);

	if (is_error)
	{
		globals::error_handler.log_lexical_error(
			"Invalid number format '" + lexeme_str + "'",
			line,
			start_column,
			start_position,
			true
		);

		return make_tk(
			Token::Type::T_ERROR,
			lexeme,
			start_column,
			start_position
		);
	}

	// Remove the trailing dot if it exists
	if (has_dot && lexeme.back() == '.')
	{
		lexeme.remove_suffix(1);
		// Move the position back by 1 so that the dot is a separate token
		retreat();
		has_dot = false;
	}
	
	return make_tk(
		has_dot ? Token::Type::T_REAL_LITERAL : Token::Type::T_INTEGER_LITERAL,
		lexeme,
		start_column,
		start_position
	);
}

std::unique_ptr<Token> Lexer::make_operator_tk(void)
{
	size_t lexeme_length = 1; // 1 because we already have the first character
	size_t start_column = column, start_position = pos;
	
	advance(); // Move to the next character
	char curr = peek();
	// We will only consider operators with a maximum of 3 characters
	while (!at_eof() && is_operator_sym(curr) && lexeme_length <= 3)
	{
		lexeme_length++;
		advance();
		curr = peek();
	}
	
	std::string_view lexeme = source.substr(start_position, lexeme_length);
	return make_tk(
		Token::Type::T_OPERATOR_SYMBOL,
		lexeme,
		start_column,
		start_position
	);
}

std::unique_ptr<Token> Lexer::make_tk(Token::Type type, std::string_view lexeme, size_t start_column, size_t start_position)
{
	auto tk = std::make_unique<Token>(
		type,
		lexeme,
		line,
		start_column,
		start_position
	);

	std::cout << *tk;
	return tk;
}

bool Lexer::skip_whitespaces()
{
	while (!at_eof())
	{
		switch(peek())
		{
			case ' ': case '\t': case '\r': case '\v': case '\f':
				advance();
				break;
			case '\n':
				advance();
				return true;
			case '#':
				advance(); // Skip the '#'
				skip_comment();
				break;

			default: return false;
		}
	}

	return false;
}
void Lexer::skip_comment()
{
	while (!at_eof() && !at_eol())
	{
		advance();
	}
}

char Lexer::advance(size_t offset)
{
	//if (0 <= pos + offset && pos + offset <= source.size())
	//{
	//	char c = source[pos];
	//	pos += offset;
	//	return c;
	//}
	//pos = source.size() + 1;
	//return '\0';

	for (size_t i = 0; i < offset; i++)
	{
		if (at_eof())
		{
			return '\0';
		}

		if (source[pos] == '\n')
		{
			line++;
			column = 1;
		}
		else
		{
			column++;
		}
		pos++;
	}

	return source[pos - 1];
}

//! Temporary function to lex the entire source code
void Lexer::lex_source(std::string_view source)
{
	// This is a temporary function to lex the entire source code
	this->source = source;
	while (!at_eof())
	{
		std::unique_ptr<Token> token = scan_tk();
		if (token != nullptr)
		{
			std::cout << *token << '\n';
		}
	}

	reset();

	// Report errors if there are any
	globals::error_handler.check_errors();
}

bool is_operator_sym(char c)
{
	switch (c)
	{
	case '+': case '-': case '*': case '/': case '%': case '^': case '!':
	case '=': case '<': case '>': case '&': case '|': case '~': case '?':
	case ':': case '@': case '$':
		return true;
	default:
		return false;
	}
}