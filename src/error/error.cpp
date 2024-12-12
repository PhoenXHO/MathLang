#include <iostream>

#include "error/error.hpp"
#include "util/config.hpp" // for `config::repl_mode`
#include "util/globals.hpp" // for `globals::source`


// To make the error the most readable, we use the following format:
// <ERROR_TYPE>: message at line <line>, column <column>
// The error type will have the red color for an error and yellow for a warning.

// The error message will be followed by a suggestion, if any.
//  	<source line>
//  	<caret pointing to the exact column with squiggly lines>

//TODO: Handle multiline errors
void Error::set_message(std::string_view message, std::string_view suggestion, std::string_view extra_info)
{
	std::string_view type_str;
	switch (m_type)
	{
		case Type::LEXICAL: type_str = "\033[1;31mLEXICAL_ERROR"; break;
		case Type::SYNTAX: type_str = "\033[1;31mSYNTAX_ERROR"; break;
		case Type::SEMANTIC: type_str = "\033[1;31mSEMANTIC_ERROR"; break;
		case Type::COMPILETIME: type_str = "\033[1;31mCOMPILETIME_ERROR"; break;
		case Type::RUNTIME: type_str = "\033[1;31mRUNTIME_ERROR"; break;
		case Type::WARNING: type_str = "\033[1;33mWARNING"; break;
	}

	this->message += std::string(type_str) + ":\033[0m " + std::string(message);
	this->message += " at line " + std::to_string(m_location.line) + ", column " + std::to_string(m_location.column) + '\n';
	this->message += suggestion.empty() ? "" : (std::string(suggestion) + '\n');

	// Print the current line
	std::string line;
	if (m_location.line > 0 && m_location.line <= globals::source.size())
	{
		size_t start = globals::source.rfind('\n', m_location.position) + 1;
		size_t end = globals::source.find('\n', m_location.position);
		end = end == std::string::npos ? globals::source.size() : end;
		line = globals::source.substr(start, end - start);
	}

	this->message += + "\t" + line + '\n';
	
	// Print the caret
	this->message += "\t";
	for (size_t i = 0; i < m_location.column - 1; i++)
	{
		this->message += ' ';
	}
	this->message += "^";

	// Print the squiggly lines
	for (size_t i = 0; m_length > 1 && i < m_length - 1; i++)
	{
		this->message += '~';
	}
	this->message += '\n';
	this->message += extra_info.empty() ? "" : (std::string(extra_info) + '\n');
}

void ErrorArray::log_error(Error error)
{
	errors.push_back(error);
}

void ErrorArray::log_warning(Error error)
{
	errors.push_back(error);
	warning_count++;
}

void ErrorArray::report_errors(void) const
{
	for (const auto & error : errors)
	{
		std::cerr << error.what();
	}
}