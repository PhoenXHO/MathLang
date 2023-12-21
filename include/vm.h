#ifndef VM_H
#define VM_H

#include <string_view>
#include <memory>
#include <stack>

#include "compiler.h"
#include "mathobj.h"

class VM
{
private:
	std::string_view source;
	std::unique_ptr<Compiler> compiler;
	std::stack<std::shared_ptr<MathObj>> stack;

public:
	VM(std::string_view source);

	void interpret_source(void);
	void run(void);
};

#endif // VM_H