#ifndef VM_H
#define VM_H

#include <string_view>
#include <memory>
#include <stack>

#include "compiler.h"
#include "mathobj.h"
#include "scope.h"

class VM
{
private:
	std::unique_ptr<Compiler> compiler;
	std::stack<std::shared_ptr<MathObj>> stack;
	std::shared_ptr<Scope> current_scope;

public:
	VM() : current_scope(std::make_shared<Scope>()) {}

	void interpret_source(std::string_view source);
	void run(void);
};

#endif // VM_H