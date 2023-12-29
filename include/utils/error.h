#ifndef ERROR_H
#define ERROR_H

#include <string_view>
#include <vector>
#include <memory>

#include "token.h"

struct Error;
struct LexicalError;
struct SyntaxError;
struct ErrorHandler;

struct Error
{
	enum class ErrorType
	{
		LEXICAL_ERR,
		SYNTAX_ERR,
		SEMANTIC_ERR,
		RUNTIME_ERR
	};

	ErrorType type(void) { return _type_; }
	std::string message(void) { return _message_; }
	size_t line(void) { return _line_; }
	size_t column(void) { return _column_; }
	size_t position(void) { return _pos_; }

	virtual std::string_view get_additional_info(void) const = 0;
	virtual size_t length(void) const
	{ return _len_; }

protected:
	Error(ErrorType type, std::string msg, size_t l, size_t c, size_t pos, size_t len) :
		_type_(type),
		_message_(msg),
		_line_(l),
		_column_(c),
		_pos_(pos),
		_len_(len)
	{}

	ErrorType _type_;
	std::string _message_;
	size_t _line_; // Line number where the error occurred
	size_t _column_; // Column number where the error occurred
	size_t _pos_; // Index of the error
	size_t _len_;
};
typedef Error::ErrorType ErrorType;

struct LexicalError : Error
{
	LexicalError(std::string_view lexeme, std::string msg, size_t l, size_t c, size_t pos) :
		Error(ErrorType::LEXICAL_ERR, msg, l, c, pos, lexeme.length()),
		lexeme(lexeme)
	{}

	std::string_view get_additional_info(void) const override
	{ return lexeme; }

private:
	std::string_view lexeme;
};

struct SyntaxError : Error
{
	SyntaxError(std::string msg, size_t l, size_t c, size_t pos, size_t len) :
		Error(ErrorType::SYNTAX_ERR, msg, l, c, pos, len)
	{}

	std::string_view get_additional_info(void) const override
	{ return ""; }
};

struct SemanticError : Error
{
	std::string additional_info;

	SemanticError(std::string msg, std::string additional_info, size_t l, size_t c, size_t pos, size_t len) :
		Error(ErrorType::SEMANTIC_ERR, msg, l, c, pos, len),
		additional_info(additional_info)
	{}
	
	std::string_view get_additional_info(void) const override
	{ return additional_info; }
};

class ErrorHandler
{
public:
	static bool has_errors(void);
	static void report_errors(std::string_view source);

	static void push_error(std::unique_ptr<Error> & err);

private:
	static std::vector<std::unique_ptr<Error>> errors;
};

#endif // ERROR_H