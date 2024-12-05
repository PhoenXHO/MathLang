#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <unordered_map>
#include <memory>
#include <string>

#include "symbol/symbol_table.hpp"

struct Scope
{
	Scope(std::shared_ptr<Scope> parent = nullptr) : parent(parent) {}
	~Scope() = default;

	void define_variable(std::string_view name, MathObjPtr value = nullptr)
	{ symbols.define_variable(name, value); }

	bool is_variable_defined(std::string_view name) const
	{ return symbols.get_index(name, Symbol::Type::S_VARIABLE) != -1; }

	size_t get_variable_index(std::string_view name) const;

	/// @brief Get the variable from the symbol table at the given index
	/// @param index The index of the variable
	/// @return The variable at the given index or `nullptr` if the index is out of bounds
	const std::shared_ptr<Variable> get_variable(size_t index) const
	{ return index < symbols.size() ? std::static_pointer_cast<Variable>(symbols[index]) : nullptr; }
	/// @brief Get the variable from the symbol table with the given name
	/// @param name The name of the variable
	/// @return The variable with the given name or `nullptr` if the variable is not defined
	const std::shared_ptr<Variable> get_variable(std::string_view name) const;

	void set_variable(size_t index, MathObjPtr value)
	{ symbols[index]->value = value; }

private:
	std::shared_ptr<Scope> parent;
	SymbolTable symbols;
};

#endif // SCOPE_HPP