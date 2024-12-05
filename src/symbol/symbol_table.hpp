#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <memory>

#include "object/object.hpp"
#include "symbol/symbol.hpp"
#include "symbol/variable.hpp"

class SymbolTable
{
	std::unordered_map<std::string, size_t> symbol_indices; // Map from the name of the symbol to its index in the symbol pool
	std::vector<std::shared_ptr<Symbol>> symbols; // The key is the name of the symbol and its type to avoid conflicts

public:
	SymbolTable() = default;
	~SymbolTable() = default;

	void define_variable(std::string_view name, MathObjPtr value = nullptr);
	
	const std::shared_ptr<Symbol> & find(std::string_view name, Symbol::Type type) const;
	size_t get_index(std::string_view name, Symbol::Type type) const;

	size_t size() const
	{ return symbols.size(); }
	const std::shared_ptr<Symbol> & operator[](size_t index) const
	{ return symbols.at(index); }
};

// Hash specialization for std::pair<std::string, Symbol::Type>
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

#endif // SYMBOL_TABLE_HPP