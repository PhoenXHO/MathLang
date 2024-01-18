#ifndef FUNCTION_H
#define FUNCTION_H

#include <memory>
#include <string>
#include <unordered_map>

#include "chunk.h"
#include "mathobj.h"

struct Scope;

struct Function
{
	enum class FunctionType
	{
		F_BUILTIN, F_CUSTOM
	};

	Function(
		std::string_view name,
		FunctionType type,
		MathObjType return_type
	) :
		name(name),
		type(type),
		return_type(return_type)
	{}

	FunctionType type;
	std::string name;
	std::unordered_map<std::string, MathObjType> parameters;
	MathObjType return_type;
};
typedef Function::FunctionType FunctionType;

struct CustomFunction : public Function
{
	CustomFunction(
		std::string_view name,
		MathObjType return_type
	) :
		Function(name, FunctionType::F_CUSTOM, return_type)
	{}

	std::shared_ptr<Chunk> chunk;
	std::shared_ptr<Scope> scope;
};

#endif // FUNCTION_H