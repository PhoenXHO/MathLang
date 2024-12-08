#pragma once

#include <iostream>
#include <string_view>
#include <string>
#include <exception>
#include <vector>

#include "util/util.hpp" // for `SourceLocation`


struct Error;


class ErrorArray : public std::exception
{
	std::vector<Error> errors;
	int warning_count = 0; // Count the number of warnings to exclude them from the count of errors

public:
	ErrorArray() = default;
	~ErrorArray() = default;

	void log_error(Error error);
	void log_warning(Error error);
	void report_errors(void) const;

	void clear(void)
	{ errors.clear(); }
	bool has_errors(void) const
	{ return (int)errors.size() - warning_count > 0; }
	bool has_warnings(void) const
	{ return warning_count > 0; }
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

	Error(Type type, std::string message, SourceLocation location, size_t length, std::string suggestion = "") :
		m_type(type), m_location(location), m_length(length)
	{
		set_message(message, suggestion);
	}

	void set_message(std::string_view message, std::string_view suggestion);

	Type type(void) const { return m_type; }
	SourceLocation location(void) const { return m_location; }
	size_t length(void) const { return m_length; }
	std::string_view what(void) const noexcept
	{
		return message;
	}

protected:
	Type m_type;
	std::string message;
	SourceLocation m_location;
	size_t m_length = 0;
};

// These are simply for convenience's sake and to make the code more readable.
struct LexicalError : public Error
{
	LexicalError(std::string message, SourceLocation location, size_t length, std::string suggestion = "") :
		Error(Type::LEXICAL, message, location, length, suggestion)
	{}
};
struct SyntaxError : public Error
{
	SyntaxError(std::string message, SourceLocation location, size_t length, std::string suggestion = "") :
		Error(Type::SYNTAX, message, location, length, suggestion)
	{}
};
struct SemanticError : public Error
{
	SemanticError(std::string message, SourceLocation location, size_t length, std::string suggestion = "") :
		Error(Type::SEMANTIC, message, location, length, suggestion)
	{}
};
struct CompiletimeError : public Error
{
	CompiletimeError(std::string message, SourceLocation location, size_t length, std::string suggestion = "") :
		Error(Type::COMPILETIME, message, location, length, suggestion)
	{}
};
struct RuntimeError : public Error
{
	RuntimeError(std::string message, SourceLocation location, size_t length, std::string suggestion = "") :
		Error(Type::RUNTIME, message, location, length, suggestion)
	{}
};
struct Warning : public Error
{
	Warning(std::string message, SourceLocation location, size_t length, std::string suggestion = "") :
		Error(Type::WARNING, message, location, length, suggestion)
	{}
};