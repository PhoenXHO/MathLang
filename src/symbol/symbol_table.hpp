#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "object/object.hpp" // for `MathObjPtr`
#include "symbol/symbol_registry.hpp" // for `Registry`
#include "variable/variable.hpp"
#include "function/function.hpp"
#include "operator/operator.hpp"
#include "object/none_object.hpp"
#include "class/builtins.hpp" // for `Builtins`


class SymbolTable
{
	Registry<VariablePtr> variables;
	Registry<FunctionPtr> functions;
	OperatorRegistry operators;
	Registry<ClassPtr> classes;

public:
	SymbolTable() = default;
	~SymbolTable() = default;

	#pragma region Variables
	Registry<VariablePtr> & get_variables(void)
	{ return variables; }

	VariablePtr define_variable(std::string_view name, MathObjPtr value = nullptr);

	size_t v_size() const
	{ return variables.size(); }

	/// @brief Get the variable from the symbol table at the given index
	/// @return The variable at the given index or `nullptr` if the index is out of bounds
	VariablePtr get_variable(size_t index) const
	{ return index < variables.size() ? variables[index] : nullptr; }

	/// @brief Get the variable from the symbol table with the given name
	/// @return The variable with the given name or `nullptr` if the variable is not defined
	size_t get_variable_index(std::string_view name) const
	{ return variables.get_index(name); }

	/// @brief Get the variable from the symbol table with the given name
	/// @return The variable with the given name or `nullptr` if the variable is not defined
	std::pair<size_t, VariablePtr> get_variable(std::string_view name) const
	{ return variables.find(name); }

	void set_variable(size_t index, MathObjPtr value)
	{ variables[index]->set(value); }
	#pragma endregion


	#pragma region Functions
	Registry<FunctionPtr> & get_functions(void)
	{ return functions; }

	void init_builtin_functions(void);

	FunctionPtr define_function(std::string_view name)
	{ return functions.define(name, std::make_shared<Function>(name)); }

	std::pair<size_t, FunctionPtr> find_function(std::string_view name) const
	{ return functions.find(name); }

	FunctionPtr get_function(size_t index) const
	{ return functions[index]; }
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
		Builtins::init_builtin_classes();

		MathObj::none = std::make_shared<NoneObj>();
	}

	void define_class(std::string_view name, const ClassPtr & cls)
	{ classes.define(name, cls); }

	std::pair<size_t, ClassPtr> find_class(std::string_view name) const
	{ return classes.find(name); }
	#pragma endregion
};