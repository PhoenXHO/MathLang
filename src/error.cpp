#include <iostream>

#include "error.h"
#include "globals.h"
#include "lexer.h"

std::vector<std::unique_ptr<Error>> ErrorHandler::errors;

void report_error(std::unique_ptr<Error> & err, std::string_view source);
const char * error_type_string(ErrorType type);
size_t find_previous_line_start(std::string_view source, size_t start_from);
size_t find_next_line_end(std::string_view source, size_t start_from);

void ErrorHandler::report_errors(std::string_view source)
{
	for (auto e = errors.begin(); e != errors.end(); e++)
		report_error(*e, source);

	errors.clear();
}

void report_error(std::unique_ptr<Error> & err, std::string_view source)
{
	std::string_view additional_info;

	if (err->type() == ErrorType::LEXICAL_ERR)
	{
		LexicalError * lex_err = dynamic_cast<LexicalError *>(err.get());
		if (lex_err != nullptr)
			additional_info = lex_err->get_additional_info();
		else
			throw std::logic_error("downcasting failed in `report_error()`");
	}

	std::cerr << "[error] " << file_name << ": "
		<< "line " << err->line()
		<< ", column " << err->column() << "\n"
		<< error_type_string(err->type()) << ": "
		<< err->message();
	if (additional_info != "")
		std::cerr << " : \"" << additional_info << '\"';

	size_t line_start = find_previous_line_start(source, err->position()) + 1;
	size_t line_end = find_next_line_end(source, err->position());
	
	std::string_view error_line = source.substr(line_start, line_end - line_start);
	std::cerr << "\n\n" << error_line << '\n';

	size_t num_spaces = err->column() - 1;
	std::cerr << std::string(num_spaces, ' ') << '^';
	auto err_len = err->length();
	if (err_len > 0)
		std::cout << std::string(err_len - 1, '~');
	std::cout << std::endl;
}

const char * error_type_string(ErrorType type)
{
	switch (type)
	{
		case ErrorType::LEXICAL_ERR:
			return "LEXICAL_ERROR";
		case ErrorType::SYNTAX_ERR:
			return "SYNTAX_ERROR";
		case ErrorType::SEMANTIC_ERR:
			return "SEMANTIC_ERROR";
		case ErrorType::RUNTIME_ERR:
			return "RUNTIME_ERROR";

		default: return "";
	}
}

void ErrorHandler::push_error(std::unique_ptr<Error> & err)
{ errors.push_back(std::move(err)); }

bool ErrorHandler::has_errors(void)
{ return !errors.empty(); }

size_t find_previous_line_start(std::string_view source, size_t start_from)
{
	size_t i;
	for (i = start_from; i > 0 && source[i] != '\n'; --i);
	if (i == 0)
		return -1;
	return i;
}
size_t find_next_line_end(std::string_view source, size_t start_from)
{
	size_t i;
	for (i = start_from; i < source.length() && source[i] != '\n'; ++i);
	return i;
}