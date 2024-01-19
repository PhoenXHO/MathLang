#include "scope.h"

std::unordered_map<std::string_view, std::shared_ptr<Variable>>::iterator Scope::find_variable(std::string_view name, bool local_only)
{
	auto it = variables.find(name);
	if (it != variables.end())
		return it;
	if (parent && !local_only)
		return parent->find_variable(name);
	return variables.end();
}

uint8_t Scope::find_variable_index(std::string name)
{
	auto it = variable_indices.find(name);
	if (it != variable_indices.end())
		return it->second;
	if (parent)
		return parent->find_variable_index(name);
	
	// just in case of a bug
	throw std::runtime_error("variable `" + name + "` not found");
}

MultiRange<FuncImplementations::const_iterator> Scope::get_function_implementations(std::string_view name)
{
	auto it = function_table.get_implementations(name);
	MultiRange<FuncImplementations::const_iterator> range(it.first, it.second);
	if (parent)
	{
		auto parent_range = parent->get_function_implementations(name);
		range.add_range(parent_range);
	}
	return range;
}

uint8_t Scope::find_function_index(std::string name)
{
	auto it = function_indices.find(name);
	if (it != function_indices.end())
		return it->second;
	if (parent)
		return parent->find_function_index(name);
	
	// just in case of a bug
	throw std::runtime_error("function `" + name + "` not found");
}

void push_scope(std::shared_ptr<Scope> & scope)
{
	if (!scope)
	{
		scope = std::make_shared<Scope>();
		return;
	}

	auto new_scope = std::make_shared<Scope>();
	new_scope->parent = scope;
	scope->children.push_back(new_scope);
	scope = new_scope;
}

void pop_scope(std::shared_ptr<Scope> & scope)
{
	if (!scope->parent)
		// just in case
		throw std::runtime_error("cannot leave global scope");
	leave_scope(scope);
	scope->children.erase(scope->children.begin());
}

void pop_function(std::shared_ptr<Scope> & scope)
{
	if (!scope->parent)
		// just in case
		throw std::runtime_error("cannot leave global scope");
	leave_scope(scope);
}

void enter_scope(std::shared_ptr<Scope> & scope, uint8_t index)
{
	scope = scope->children[index];
}

void enter_function(std::shared_ptr<Scope> & scope, std::shared_ptr<CustomFunction> & function)
{
	function->scope->parent = scope;
	scope = function->scope;
}

void leave_scope(std::shared_ptr<Scope> & scope)
{
	if (!scope->parent)
		// just in case
		throw std::runtime_error("cannot leave global scope");
	scope = scope->parent;
}