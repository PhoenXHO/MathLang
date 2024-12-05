#include <iostream>
#include <cstdarg>

#include "debug/verbose.hpp"
#include "util/config.hpp"

void log_v(std::string_view message, ...)
{
	if (config::verbose)
	{
		va_list args;
		va_start(args, message);
		std::cout << "\033[1;34m [VERBOSE] \033[0m";
		std::vprintf(message.data(), args);
		va_end(args);
	}
}