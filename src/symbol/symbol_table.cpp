#include "symbol/symbol_table.hpp"

void SymbolTable::define_variable(std::string_view name, MathObjPtr value)
{
	auto variable = std::make_shared<Variable>(name, value);
	variables.define(name, variable);
}