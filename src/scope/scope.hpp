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

	void define_variable(std::string_view name, MathObjPtr value = nullptr)
	{ symbols.define_variable(name, value); }

	bool is_variable_defined(std::string_view name) const
	{ return symbols.get_index(name, Symbol::Type::S_VARIABLE) != -1; }

	size_t get_variable_index(std::string_view name) const;

	const std::shared_ptr<Variable> get_variable(size_t index) const
	{ return std::static_pointer_cast<Variable>(symbols[index]); }
	const std::shared_ptr<Variable> get_variable(std::string_view name) const;

	void set_variable(size_t index, MathObjPtr value)
	{ symbols[index]->value = value; }

private:
	std::shared_ptr<Scope> parent;
	SymbolTable symbols;
};

#endif // SCOPE_HPP