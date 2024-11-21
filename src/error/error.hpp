#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>
#include <string_view>
#include <string>
#include <exception>
#include <vector>

struct Error;

class ErrorArray : public std::exception
{
	std::vector<Error> errors;

public:
	ErrorArray() = default;
	~ErrorArray() = default;

	void log_error(Error error);
	void report_errors(void) const;

	void clear(void)
	{ errors.clear(); }
	bool has_errors(void) const
	{ return !errors.empty(); }
};

struct Error
{
	enum class Type
	{
		LEXICAL,
		SYNTAX,
		SEMANTIC,
		COMPILETIME,
		RUNTIME,
		INTERNAL,
		WARNING
	};

	Error(Type type, std::string message, size_t line, size_t column, size_t position)
		: type(type), line(line), column(column), position(position)
	{
		set_message(message);
	}

	void set_message(std::string_view message);

	Type get_type(void) const { return type; }
	std::string_view what(void) const noexcept
	{
		return message;
	}

protected:
	Type type;
	std::string message;
	size_t line;
	size_t column;
	size_t position;
};

// These are simply for convenience's sake and to make the code more readable.
struct LexicalError : public Error
{
	LexicalError(std::string message, size_t line, size_t column, size_t position)
		: Error(Type::LEXICAL, message, line, column, position) {}
};
struct SyntaxError : public Error
{
	SyntaxError(std::string message, size_t line, size_t column, size_t position)
		: Error(Type::SYNTAX, message, line, column, position) {}
};
struct SemanticError : public Error
{
	SemanticError(std::string message, size_t line, size_t column, size_t position)
		: Error(Type::SEMANTIC, message, line, column, position) {}
};
struct CompiletimeError : public Error
{
	CompiletimeError(std::string message, size_t line, size_t column, size_t position)
		: Error(Type::COMPILETIME, message, line, column, position) {}
};
struct RuntimeError : public Error
{
	RuntimeError(std::string message, size_t line, size_t column, size_t position)
		: Error(Type::RUNTIME, message, line, column, position) {}
};
struct Warning : public Error
{
	Warning(std::string message, size_t line, size_t column, size_t position)
		: Error(Type::WARNING, message, line, column, position) {}
};

#endif // ERROR_HPP