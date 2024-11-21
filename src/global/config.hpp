#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace config
{
	extern bool repl_mode;
	extern bool verbose;
	extern bool dev;

	// Colors for verbose output
	extern const char * const SUCCESS_COLOR;
	extern const char * const DESTROY_COLOR;
	extern const char * const PROCESS_COLOR;
}

#endif // CONFIG_HPP