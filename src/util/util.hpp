#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstddef>

// Struct to store the line, column, and position in the source code
struct SourceLocation
{
	size_t line;
	size_t column;
	size_t position;

	SourceLocation(size_t line, size_t column, size_t position) :
		line(line), column(column), position(position)
	{}
	SourceLocation() : SourceLocation(1, 1, 0) {}
};

#endif // UTIL_HPP