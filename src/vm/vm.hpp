#ifndef VM_HPP
#define VM_HPP

#include <memory>
#include <string_view>
#include <stack>

#include "compiler/compiler.hpp"
#include "error/error_handler.hpp"

class VM
{
	// Using a string_view to avoid copying the source code,
	// since it is guaranteed to be valid for the lifetime of the VM instance
	std::string_view source;
	std::unique_ptr<Compiler> compiler;

	std::stack<MathObjPtr> stack;

public:
	VM() = default;
	~VM() = default;

	// This function will return false if there was an error
	// This will most likely be used to check for code incompleteness
	InterpretResult interpret_source(std::string_view source);
	void run(void);
};

#endif // VM_HPP