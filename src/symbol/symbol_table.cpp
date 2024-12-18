#include "symbol/symbol_table.hpp"

VariablePtr SymbolTable::define_variable(std::string_view name, ObjectPtr value)
{
	auto variable = std::make_shared<Variable>(name, value);
	return variables.define(name, variable);
}