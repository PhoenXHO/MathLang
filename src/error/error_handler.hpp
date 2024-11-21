#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <iostream>
#include <string_view>
#include <vector>

#include "error/error.hpp" // for `Error`

enum class InterpretResult
{
	// The order of these is important
	// Higher values are more severe
	OK,
	WARNING, // i.e. unused variable, unused function, etc.
	INTERNAL_ERROR, // i.e. memory allocation failure, etc.
	RUNTIME_ERROR, // i.e. division by zero, invalid memory access, etc.
	COMPILE_ERROR, // i.e. invalid operation, invalid type, etc.
	SEMANTIC_ERROR, // i.e. undeclared variable, undeclared function, etc.
	SYNTAX_ERROR, // i.e. invalid syntax, unexpected token, etc.
	LEXICAL_ERROR, // i.e. invalid character, invalid token, etc.
	INCOMPLETE_CODE // For REPL mode
};

class ErrorHandler
{
	//std::vector<Error> errors;
	ErrorArray errors;
	InterpretResult most_severe_error = InterpretResult::OK;
	bool incomplete_code = false;

	// Allow the Parser class to access the ErrorHandler class (which will only need to access the `incomplete_code` flag)
	friend class Parser;

public:
	ErrorHandler() = default;
	~ErrorHandler() = default;
	
	void reset(void)
	{
		errors.clear();
		most_severe_error = InterpretResult::OK;
		incomplete_code = false;
	}

	InterpretResult get_most_severe_error(void) const
	{ return most_severe_error; }
	bool is_incomplete_code(void) const
	{ return incomplete_code; }

	void log_error(Error error, bool fatal = false);
	// For convenience
	void log_lexical_error(std::string_view message, size_t line, size_t column, size_t position, bool fatal = false);
	void log_syntax_error(std::string_view message, size_t line, size_t column, size_t position, bool fatal = false);
	void log_semantic_error(std::string_view message, size_t line, size_t column, size_t position, bool fatal = false);
	void log_compiletime_error(std::string_view message, size_t line, size_t column, size_t position, bool fatal = false);
	void log_runtime_error(std::string_view message, size_t line, size_t column, size_t position, bool fatal = false);
	void log_warning(std::string_view message, size_t line, size_t column, size_t position, bool fatal = false);

	bool has_errors(void) const
	{ return errors.has_errors(); }

	// Function to check for errors and report them if there are any
	void check_errors(void)
	{
		if (has_errors())
		{
			throw errors;
		}
	}
};

#endif // ERROR_HANDLER_HPP