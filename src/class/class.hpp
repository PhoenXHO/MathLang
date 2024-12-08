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

	bool is_sub_class(const ClassPtr & cls, bool strict = false) const;
	/// @brief Compare the specificity of two classes.
	/// The specificity is a measure of how well a class matches another class.
	/// The higher the specificity, the better the match. If the specificity is equal,
	/// the specificity will be `2`, `1` if this class is castable to `cls`, and `0` otherwise.
	/// @param cls The class to compare with
	/// @return The specificity of the class compared to `cls`
	int measure_specificity(const ClassPtr & cls) const;

	/// @brief Instantiate a new object of this class
	std::function<MathObjPtr(const std::any &)> instantiate;
	/// @brief Cast an object to this class
	std::function<MathObjPtr(MathObjPtr)> cast;
	std::function<bool(ClassPtr)> can_cast_to;

	std::string to_string(void) const
	{
		std::ostringstream oss;
		oss << "<" << m_name << ">";
		return oss.str();
	}

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