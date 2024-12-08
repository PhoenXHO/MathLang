#pragma once

#include <string_view>

#include "error/error_handler.hpp"


namespace globals
{
	extern std::string_view source;
	extern ErrorHandler error_handler;
}