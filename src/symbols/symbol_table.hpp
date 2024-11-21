#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <memory>

#include "object/object.hpp"

struct Symbol
{
	enum class Type
	{
		S_NONE,
		S_VARIABLE
		// For now, we only have variables
	};

	Symbol(std::string name, Type type, std::shared_ptr<void> value)
		: name(name), type(type), value(value) {}

	std::string_view get_name(void) const
	{ return name; }
	Type get_type(void) const
	{ return type; }
	const std::shared_ptr<void> & get_value(void) const
	{ return value; }

private:
	std::string name;
	Type type;
	std::shared_ptr<void> value; // The value can be a MathObjPtr, FunctionPtr, or ClassPtr, depending on the type
};

class SymbolTable
{
	std::unordered_map<std::string_view, Symbol> symbols; // The key is the name of the symbol, which is why we use a string_view, since the string is already stored in the symbol

public:
	SymbolTable() = default;
	~SymbolTable() = default;

	void define_variable(std::string_view name, MathObjPtr value)
	{
		symbols.emplace(name, Symbol(std::string(name), Symbol::Type::S_VARIABLE, value));
	}

	const Symbol & get_symbol(std::string_view name) const
	{
		auto it = symbols.find(name);
		// If the symbol is not found, return an empty symbol (we don't want to throw an error here, but we want to handle it in the calling function)
		if (it == symbols.end())
		{
			return Symbol("", Symbol::Type::S_NONE, nullptr);
		}
		return it->second;
	}
};

#endif // SYMBOL_TABLE_HPP