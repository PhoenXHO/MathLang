#include "global/config.hpp"

namespace config
{
	bool repl_mode = false;
	bool verbose   = false;
	bool dev       = false;
	bool print_all = false;

	// Colors for verbose output
	const char * const SUCCESS_COLOR = "\033[1;32m";
	const char * const DESTROY_COLOR = "\033[1;33m";
	const char * const PROCESS_COLOR = "\033[1;34m";
}