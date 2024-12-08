#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <any>
#include <functional>
#include <utility> // for `std::pair`

#include "variable/variable.hpp"
#include "symbol/symbol_registry.hpp" // for `Registry`


struct Class;
using ClassPtr = std::shared_ptr<Class>;


struct Class : public std::enable_shared_from_this<Class>
{
	Class(std::string name) : m_name(name) {}
	~Class() = default;

	const std::string & name(void) const
	{ return m_name; }

	void add_field(const VariablePtr & variable)
	{
		fields.define(variable->name(), variable);
	}

	VariablePtr get_field(std::string_view name) const
	{ return fields[name]; }

	bool can_cast_to(const ClassPtr & cls) const;

	//virtual MathObjPtr instantiate(const std::any & value) const = 0;
	std::function<MathObjPtr(const std::any &)> instantiate;

private:
	std::string m_name;
	Registry<VariablePtr> fields; // Fields
	//std::vector<ClassPtr> bases; // Inheritance (for later)
};

// Hash specialization for std::pair<ClassPtr, ClassPtr>
namespace std
{
	template <>
	struct hash<std::pair<ClassPtr, ClassPtr>>
	{
		size_t operator()(const std::pair<ClassPtr, ClassPtr> & pair) const
		{
			auto hash1 = std::hash<ClassPtr>()(pair.first);
			auto hash2 = std::hash<ClassPtr>()(pair.second);
			return hash1 ^ (hash2 << 1);
		}
	};
}