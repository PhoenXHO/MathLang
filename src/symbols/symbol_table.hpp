#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <memory>

#include "object/object.hpp"
#include "symbols/symbol.hpp"
#include "symbols/variable.hpp"

namespace std
{
	template<>
	struct hash<std::pair<std::string, Symbol::Type>>
	{
		size_t operator()(const std::pair<std::string, Symbol::Type> & key) const
		{
			auto hash1 = std::hash<std::string>{}(key.first);
			auto hash2 = std::hash<int>{}(static_cast<int>(key.second));
			return hash1 ^ (hash2 << 1); // Combine the two hashes
		}
	};
}

class SymbolTable
{
	std::unordered_map<std::string, size_t> symbol_indices; // Map from the name of the symbol to its index in the symbol pool
	std::vector<std::shared_ptr<Symbol>> symbols; // The key is the name of the symbol and its type to avoid conflicts

public:
	SymbolTable() = default;
	~SymbolTable() = default;

	void define_variable(std::string_view name, MathObjPtr value = nullptr)
	{
		std::string name_str(name);
		auto symbol = Variable(name, value);
		symbols.push_back(std::make_shared<Symbol>(symbol));
		symbol_indices[name_str] = symbols.size() - 1;
	}

	const std::shared_ptr<Symbol> & find(std::string_view name, Symbol::Type type) const
	{
		size_t index = get_index(name, type);
		if (index != -1)
		{
			return symbols[index];
		}
		return Symbol::empty_symbol;
	}

	size_t get_index(std::string_view name, Symbol::Type type) const
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

	const std::shared_ptr<Symbol> & operator[](size_t index) const
	{
		return symbols.at(index);
	}
};

#endif // SYMBOL_TABLE_HPP