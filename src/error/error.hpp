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

struct ErrorInfo
{
	std::string message;
	std::string suggestion;
	std::string extra_info;
	SourceLocation location;
	size_t length;

	ErrorInfo(
		std::string_view message,
		SourceLocation location,
		size_t length,
		std::string_view suggestion = "",
		std::string_view extra_info = ""
	) :
		message(message),
		location(location),
		length(length),
		suggestion(suggestion),
		extra_info(extra_info)
	{}
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

	Error(Type type, ErrorInfo info) :
		m_type(type), m_location(info.location), m_length(info.length)
	{
		set_message(info.message, info.suggestion, info.extra_info);
	}

	void set_message(std::string_view message, std::string_view suggestion, std::string_view extra_info);

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
	LexicalError(ErrorInfo info) :
		Error(Type::LEXICAL, info)
	{}
};
struct SyntaxError : public Error
{
	SyntaxError(ErrorInfo info) :
		Error(Type::SYNTAX, info)
	{}
};
struct SemanticError : public Error
{
	SemanticError(ErrorInfo info) :
		Error(Type::SEMANTIC, info)
	{}
};
struct CompiletimeError : public Error
{
	CompiletimeError(ErrorInfo info) :
		Error(Type::COMPILETIME, info)
	{}
};
struct RuntimeError : public Error
{
	RuntimeError(ErrorInfo info) :
		Error(Type::RUNTIME, info)
	{}
};
struct Warning : public Error
{
	Warning(ErrorInfo info) :
		Error(Type::WARNING, info)
	{}
};