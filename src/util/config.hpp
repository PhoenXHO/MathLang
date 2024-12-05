#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace config
{
	extern bool repl_mode; // Whether the program is in REPL mode
	extern bool verbose; // Whether to print verbose output
	extern bool dev; // Whether to enable developer mode
	extern bool print_all; // Whether to print all expressions that do not have a semicolon at the end or just the last one
	extern bool benchmark; // Whether to enable benchmark mode which prints the time taken to execute the program
	extern bool warnings; // Whether to enable warnings

	// Colors for verbose output
	extern const char * const SUCCESS_COLOR;
	extern const char * const DESTROY_COLOR;
	extern const char * const PROCESS_COLOR;
}

#endif // CONFIG_HPP