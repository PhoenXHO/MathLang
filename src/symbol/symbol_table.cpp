#include "symbol/symbol_table.hpp"

void SymbolTable::define_variable(std::string_view name, MathObjPtr value)
{
	std::string name_str(name);
	auto symbol = Variable(name, value);
	symbols.push_back(std::make_shared<Symbol>(symbol));
	symbol_indices[name_str] = symbols.size() - 1;
}

const std::shared_ptr<Symbol> & SymbolTable::find(std::string_view name, Symbol::Type type) const
{
	size_t index = get_index(name, type);
	if (index != -1)
	{
		return symbols[index];
	}
	return Symbol::empty_symbol;
}

size_t SymbolTable::get_index(std::string_view name, Symbol::Type type) const
{
	// Get all symbols with the same name
	auto range = symbol_indices.equal_range(std::string(name));
	for (auto it = range.first; it != range.second; ++it)
	{
		if (symbols[it->second]->get_type() == type)
		{
			// Return the first symbol with the correct type
			// (there should only be one symbol with this name and type)
			return it->second;
		}
	}
	return -1;
}