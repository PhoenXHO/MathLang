#pragma once

#include <memory>
#include <string_view>
#include <stack>

#include "compiler/compiler.hpp"
#include "error/error_handler.hpp"


class VM
{
	std::stack<MathObjPtr> stack;
	ConstantPool constant_pool; //TODO: Optimize storage of constants to storing duplicates
	std::shared_ptr<Scope> global_scope;
	std::shared_ptr<Scope> current_scope = global_scope;

	std::unique_ptr<Compiler> compiler;
	Chunk chunk;

public:
	VM() :
		chunk("<main>", constant_pool, global_scope),
		global_scope(std::make_shared<Scope>()),
		compiler(std::make_unique<Compiler>(chunk, constant_pool, global_scope))
	{
		global_scope->init_global_scope();
	}
	~VM() = default;

	// This function will return false if there was an error
	// This will most likely be used to check for code incompleteness
	InterpretResult interpret_source(bool interrupted = false);
	void run(void);
};