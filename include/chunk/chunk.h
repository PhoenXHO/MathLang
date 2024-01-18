#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <memory>
#include <string_view>
#include <string>
#include <cstdint>

#include "mathobj.h"
#include "operator.h"

struct Function;

struct Chunk
{
	Chunk(std::string_view name) : parent(nullptr), name(name) {}

	std::shared_ptr<Chunk> parent;
	std::string name;
	std::vector<uint8_t> bytes;
	std::vector<uint8_t>::const_iterator ip;
	std::vector<std::shared_ptr<MathObj>> constants;

	const std::vector<uint8_t> & bytecode(void) { return bytes; }
};

#endif // CHUNK_H