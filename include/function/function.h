#ifndef FUNCTION_H
#define FUNCTION_H

#include <memory>
#include <string>
#include <unordered_map>

#include "chunk.h"
#include "mathobj.h"

struct Scope;
struct Function;
// Represents a multimap of function implementations with the key being the name of the function and its arity
using FuncImplementations = std::unordered_multimap<std::string, std::shared_ptr<Function>>;

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

	inline size_t arity() const { return parameters.size(); }

	FunctionType type;
	std::string name;
	std::vector<std::pair<std::string, MathObjType>> parameters;
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

class FunctionTable
{
	FuncImplementations functions;

public:
	std::pair<FuncImplementations::const_iterator, FuncImplementations::const_iterator> get_implementations(std::string_view name) const;

	void register_function(std::string name, std::shared_ptr<Function> func);
	void register_implementation(std::string name, std::shared_ptr<Function> func);

	//void register_builtin_functions(void);
};

#endif // FUNCTION_H