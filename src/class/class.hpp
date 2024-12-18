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
	friend Object;
	
	Class(std::string name, std::vector<ClassPtr> bases = {}) :
		m_name(name), bases(bases)
	{}
	~Class() = default;

	virtual void init(void) = 0;

	const std::string & name(void) const
	{ return m_name; }

	Registry<VariablePtr> & properties(void)
	{ return m_properties; }

	VariablePtr get_property(std::string_view name) const
	{ return m_properties[name].second; }

	void add_property(const VariablePtr & variable)
	{
		m_properties.define(variable->name(), variable);
	}

	bool is_sub_class(const ClassPtr & cls, bool strict = false) const;
	/// @brief Compare the specificity of two classes.
	/// The specificity is a measure of how well a class matches another class.
	/// The higher the specificity, the better the match.
	/// @return `2` if the classes are the same, `1` if this class is castable to `cls`, and `0` otherwise
	int measure_specificity(const ClassPtr & cls) const;

	/// @brief Instantiate a new object of this class
	virtual ObjectPtr instantiate(const std::any & value) const = 0;
	/// @brief Get the default value of this class
	virtual ObjectPtr default_value(void) const = 0;
	virtual bool can_cast_to(const ClassPtr & cls) const = 0;
	/// @brief Cast an object to this class
	virtual ObjectPtr cast(const ObjectPtr & obj) const = 0;

	virtual std::string to_string(void) const
	{
		std::ostringstream oss;
		oss << "<" << m_name << ">";
		return oss.str();
	}

protected:
	std::string m_name;
	Registry<VariablePtr> m_properties; // Properties of the class
	std::vector<ClassPtr> bases; // Inheritance
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