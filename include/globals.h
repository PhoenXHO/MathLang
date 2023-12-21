#ifndef GLOBALS_H
#define GLOBALS_H

#include <string_view>
#include <memory>

#include "lexer.h"
#include "parser.h"
#include "compiler.h"

#define PRINT_LEXER_OUTPUT

struct config
{
	static bool print_lexer_output;
	static bool print_parser_output;
	static bool print_compiler_output;
};

extern std::string_view file_name;

extern std::shared_ptr<Compiler> compiler;

#endif // GLOBALS_H