#include "scope/scope.hpp"

size_t Scope::get_variable_index(std::string_view name) const
{
	// Check the current scope
	size_t variable_index = symbols.get_variable_index(name);
	if (variable_index != -1)
	{
		return variable_index;
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
		return symbols.get_variable(index);
	}
	return nullptr;
}