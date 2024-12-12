#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

#include "function/function_implementation.hpp"
#include "object/object.hpp"


class Function;
using FunctionPtr = std::shared_ptr<Function>;

struct FunctionImplentationRegistry
{
	std::unordered_map<FunctionSignature, size_t> implementation_indices;
	std::vector<FunctionImplentationPtr> implementations;

	FunctionImplentationRegistry() = default;
	~FunctionImplentationRegistry() = default;

	FunctionImplentationPtr define(const FunctionImplentationPtr & implementation);
	/// @brief Find the most specific implementation of the function for the given signature
	/// @return A pair containing the index of the implementation of the function
	/// and the implementation itself, or `{ -1, nullptr }` if no implementation is found
	std::pair<size_t, FunctionImplentationPtr> find_most_specific(const FunctionSignature & signature) const;

	FunctionImplentationPtr operator[](size_t index) const
	{ return implementations[index]; }

private:
	int measure_specificity(const FunctionSignature & signature, const FunctionSignature & other) const;
};

class Function
{
	std::string name; // Name of function
	FunctionImplentationRegistry m_implementations;

public:
	Function(std::string_view name) :
		name(name)
	{}
	~Function() = default;

	const std::string & get_name() const
	{ return name; }
	FunctionImplentationRegistry & implementations()
	{ return m_implementations; }
	
	FunctionImplentationPtr define(const FunctionImplentationPtr & implementation)
	{ return m_implementations.define(implementation); }

	/// @brief Find the most specific implementation of the function for the given signature
	/// @return The most specific implementation of the function or `nullptr` if no implementation is found
	std::pair<size_t, FunctionImplentationPtr> find_implentation(const FunctionSignature & signature) const
	{ return m_implementations.find_most_specific(signature); }

	FunctionImplentationPtr get_implementation(size_t index) const
	{ return m_implementations[index]; }

	std::string to_string() const
	{ return name + "()"; }
};