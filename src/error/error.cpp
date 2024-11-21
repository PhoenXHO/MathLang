#include <iostream>

#include "error/error.hpp"
#include "global/config.hpp" // for `config::repl_mode`

// To make the error the most readable, we use the following format:
// <ERROR_TYPE>: message at line <line>, column <column>
// The error type will have the red color for an error and yellow for a warning.

// Later, we will add the source line and point to the exact column where the error occurred, like this:
//  	<source line>
//  	<caret pointing to the exact column>
// Squiggly lines might also be added to the source line to indicate the error, but only for the lexical errors.

void Error::set_message(std::string_view message)
{
	std::string_view type_str;
	switch (type)
	{
		case Type::LEXICAL: type_str = "LEXICAL_ERROR"; break;
		case Type::SYNTAX: type_str = "SYNTAX_ERROR"; break;
		case Type::SEMANTIC: type_str = "SEMANTIC_ERROR"; break;
		case Type::COMPILETIME: type_str = "COMPILETIME_ERROR"; break;
		case Type::RUNTIME: type_str = "RUNTIME_ERROR"; break;
		case Type::WARNING: type_str = "WARNING"; break;
	}

	this->message += "\033[1;31m" + std::string(type_str) + ":\033[0m " + std::string(message);
	this->message += " at line " + std::to_string(line) + ", column " + std::to_string(column) + '\n';
}

void ErrorArray::log_error(Error error)
{
	errors.push_back(error);
}

void ErrorArray::report_errors(void) const
{
	for (const auto & error : errors)
	{
		std::cerr << error.what();
	}
}