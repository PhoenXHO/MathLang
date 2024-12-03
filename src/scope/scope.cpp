#include "scope/scope.hpp"

size_t Scope::get_variable_index(std::string_view name) const
{
	// Check the current scope
	size_t symbol_index = symbols.get_index(name, Symbol::Type::S_VARIABLE);
	if (symbol_index != -1)
	{
		return symbol_index;
	}

	// Check the parent scope
	if (parent)
	{
		return parent->get_variable_index(name);
	}
	
	return -1;
}

const std::shared_ptr<Variable> Scope::get_variable(std::string_view name) const
{
	size_t index = get_variable_index(name);
	if (index != -1)
	{
		return std::static_pointer_cast<Variable>(symbols[index]);
	}
	
	return nullptr;
}