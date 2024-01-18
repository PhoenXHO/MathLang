#ifndef SCOPE_H
#define SCOPE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "mathobj.h"
#include "function.h"

struct Scope {
	Scope() : parent(nullptr) {}

	bool is_function_scope = false;

	std::shared_ptr<Scope> parent;
	std::vector<std::shared_ptr<Scope>> children;
	std::unordered_map<std::string_view, std::shared_ptr<Variable>> variables;
	std::unordered_map<std::string, uint8_t> variable_indices;
	std::unordered_map<std::string_view, std::shared_ptr<CustomFunction>> functions;
	std::unordered_map<std::string, uint8_t> function_indices;

	std::unordered_map<std::string_view, std::shared_ptr<Variable>>::iterator find_variable(std::string_view name, bool local_only = false);
	uint8_t find_variable_index(std::string name);
	std::unordered_map<std::string_view, std::shared_ptr<CustomFunction>>::iterator find_function(std::string_view name);
	uint8_t find_function_index(std::string name);
};

void push_scope(std::shared_ptr<Scope> & scope);
void pop_scope(std::shared_ptr<Scope> & scope);
void pop_function(std::shared_ptr<Scope> & scope);
void enter_scope(std::shared_ptr<Scope> & scope, uint8_t index);
void enter_function(std::shared_ptr<Scope> & scope, std::shared_ptr<CustomFunction> & function);
void leave_scope(std::shared_ptr<Scope> & scope);

#endif // SCOPE_H