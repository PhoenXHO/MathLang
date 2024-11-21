#include <string>

#include "error/error_handler.hpp" // for `ErrorHandler`
#include "global/config.hpp" // for `config::verbose`
#include "global/globals.hpp" // for `error_handler`

namespace globals
{
	ErrorHandler error_handler;
}

void ErrorHandler::log_error(Error error, bool fatal)
{
	errors.log_error(error);

	switch (error.get_type())
	{
	case Error::Type::LEXICAL:
		if (most_severe_error < InterpretResult::LEXICAL_ERROR)
			most_severe_error = InterpretResult::LEXICAL_ERROR;
		break;

	case Error::Type::SYNTAX:
		if (most_severe_error < InterpretResult::SYNTAX_ERROR)
			most_severe_error = InterpretResult::SYNTAX_ERROR;
		break;
	case Error::Type::SEMANTIC:
		if (most_severe_error < InterpretResult::SEMANTIC_ERROR)
			most_severe_error = InterpretResult::SEMANTIC_ERROR;
		break;
	case Error::Type::COMPILETIME:
		if (most_severe_error < InterpretResult::COMPILE_ERROR)
			most_severe_error = InterpretResult::COMPILE_ERROR;
		break;
	case Error::Type::RUNTIME:
		if (most_severe_error < InterpretResult::RUNTIME_ERROR)
			most_severe_error = InterpretResult::RUNTIME_ERROR;
		break;
	case Error::Type::INTERNAL:
		if (most_severe_error < InterpretResult::INTERNAL_ERROR)
			most_severe_error = InterpretResult::INTERNAL_ERROR;
		break;
	case Error::Type::WARNING:
		if (most_severe_error < InterpretResult::OK)
			most_severe_error = InterpretResult::OK;
		break;

	default:
		break;
	}

	if (fatal)
	{
		throw errors;
	}
}
void ErrorHandler::log_lexical_error(std::string_view message, size_t line, size_t column, size_t position, bool fatal)
{
	log_error(LexicalError(std::string(message), line, column, position), fatal);
}
void ErrorHandler::log_syntax_error(std::string_view message, size_t line, size_t column, size_t position, bool fatal)
{
	log_error(SyntaxError(std::string(message), line, column, position), fatal);
}
void ErrorHandler::log_semantic_error(std::string_view message, size_t line, size_t column, size_t position, bool fatal)
{
	log_error(SemanticError(std::string(message), line, column, position), fatal);
}
void ErrorHandler::log_compiletime_error(std::string_view message, size_t line, size_t column, size_t position, bool fatal)
{
	log_error(CompiletimeError(std::string(message), line, column, position), fatal);
}
void ErrorHandler::log_runtime_error(std::string_view message, size_t line, size_t column, size_t position, bool fatal)
{
	log_error(RuntimeError(std::string(message), line, column, position), fatal);
}
void ErrorHandler::log_warning(std::string_view message, size_t line, size_t column, size_t position, bool fatal)
{
	log_error(Warning(std::string(message), line, column, position), fatal);
}