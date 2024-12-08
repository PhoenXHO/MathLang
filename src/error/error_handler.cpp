#include <string>

#include "error/error_handler.hpp" // for `ErrorHandler`
#include "util/config.hpp" // for `config::verbose`
#include "util/globals.hpp" // for `error_handler`


namespace globals
{
	ErrorHandler error_handler;
}

void ErrorHandler::log_error(Error error, bool fatal)
{
	switch (error.type())
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
		errors.log_warning(error);
		return;

	default:
		break;
	}

	errors.log_error(error);
	if (fatal)
	{
		throw errors;
	}
}
void ErrorHandler::log_lexical_error(std::string_view message, SourceLocation location, size_t length, bool fatal, std::string_view suggestion)
{
	log_error(LexicalError(std::string(message), location, length, std::string(suggestion)), fatal);
}
void ErrorHandler::log_syntax_error(std::string_view message, SourceLocation location, size_t length, bool fatal, std::string_view suggestion)
{
	log_error(SyntaxError(std::string(message), location, length, std::string(suggestion)), fatal);
}
void ErrorHandler::log_semantic_error(std::string_view message, SourceLocation location, size_t length, bool fatal, std::string_view suggestion)
{
	log_error(SemanticError(std::string(message), location, length, std::string(suggestion)), fatal);
}
void ErrorHandler::log_compiletime_error(std::string_view message, SourceLocation location, size_t length, bool fatal, std::string_view suggestion)
{
	log_error(CompiletimeError(std::string(message), location, length, std::string(suggestion)), fatal);
}
void ErrorHandler::log_runtime_error(std::string_view message, SourceLocation location, size_t length, bool fatal, std::string_view suggestion)
{
	log_error(RuntimeError(std::string(message), location, length, std::string(suggestion)), fatal);
}
void ErrorHandler::log_warning(std::string_view message, SourceLocation location, size_t length, std::string_view suggestion)
{
	if (!config::warnings)
		return;
	log_error(Warning(std::string(message), location, length, std::string(suggestion)));
}