#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "object/object.hpp" // for `MathObjPtr`
#include "variable/variable.hpp"
#include "symbol/symbol_registry.hpp" // for `Registry`
#include "operator/operator.hpp"
#include "class/class.hpp"


class SymbolTable
{
	Registry<VariablePtr> variables;
	OperatorRegistry operators;
	Registry<ClassPtr> classes;

public:
	SymbolTable() = default;
	~SymbolTable() = default;

	#pragma region Variables
	Registry<VariablePtr> & get_variables(void)
	{ return variables; }

	void define_variable(std::string_view name, MathObjPtr value = nullptr);

	size_t v_size() const
	{ return variables.size(); }

	/// @brief Get the variable from the symbol table at the given index
	/// @param index The index of the variable
	/// @return The variable at the given index or `nullptr` if the index is out of bounds
	const VariablePtr get_variable(size_t index) const
	{ return index < variables.size() ? variables[index] : nullptr; }

	/// @brief Get the variable from the symbol table with the given name
	/// @param name The name of the variable
	/// @return The variable with the given name or `nullptr` if the variable is not defined
	size_t get_variable_index(std::string_view name) const
	{ return variables.get_index(name); }

	/// @brief Get the variable from the symbol table with the given name
	/// @param name The name of the variable
	/// @return The variable with the given name or `nullptr` if the variable is not defined
	const VariablePtr get_variable(std::string_view name) const
	{ return variables.find(name); }

	void set_variable(size_t index, MathObjPtr value)
	{ variables[index]->set(value); }
	#pragma endregion


	#pragma region Operators
	OperatorRegistry & get_operators(void)
	{ return operators; }

	void init_builtin_operators(void)
	{
		operators.register_builtin_operators();
	}

	OperatorPtr find_operator(std::string_view symbol, bool is_unary = false) const
	{ return operators.find(symbol, is_unary); }
	#pragma endregion


	#pragma region Classes
	Registry<ClassPtr> & get_classes(void)
	{ return classes; }

	void init_builtin_classes(void)
	{
		define_class("MathObj", Builtins::MathObjClass::init());
		//define_class("Reference", Builtins::ReferenceClass::init());
		//define_class("None", Builtins::NoneClass::init());

		define_class("Integer", Builtins::IntegerClass::init());
		define_class("Real", Builtins::RealClass::init());
	}

	void define_class(std::string_view name, const ClassPtr & cls)
	{ classes.define(name, cls); }

	ClassPtr find_class(std::string_view name) const
	{ return classes.find(name); }
	#pragma endregion
};