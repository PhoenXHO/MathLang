#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <unordered_map>
#include <memory>
#include <string>

#include "symbols/symbol_table.hpp"

struct Scope
{
	Scope(std::shared_ptr<Scope> parent = nullptr) : parent(parent) {}
	~Scope() = default;

	void define_variable(std::string_view name, MathObjPtr value)
	{
		symbols.define_variable(name, value);
	}

	const Symbol & get_symbol(std::string_view name) const
	{
		const Symbol & symbol = symbols.get_symbol(name); // Will return an empty symbol if the symbol is not found
		// If the symbol is not found in the current scope, check the parent scope
		if (symbol.get_type() == Symbol::Type::S_NONE && parent)
		{
			return parent->get_symbol(name);
		}
		// Return the symbol found in the current scope whether it is empty or not
		return symbol;
	}

private:
	std::shared_ptr<Scope> parent;
	SymbolTable symbols;
};

#endif // SCOPE_HPP