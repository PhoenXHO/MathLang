#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <string_view>

#include "error/error_handler.hpp"

namespace globals
{
	extern std::string_view source;
	extern ErrorHandler error_handler;
}

#endif // GLOBALS_HPP