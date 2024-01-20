#ifndef VM_H
#define VM_H

#include <string_view>
#include <memory>
#include <stack>

#include "chunk.h"
#include "mathobj.h"
#include "scope.h"

class VM
{
private:
	std::shared_ptr<Chunk> chunk;
	std::stack<std::shared_ptr<MathObj>> stack;
	std::shared_ptr<Scope> current_scope;

public:
	VM() :
		current_scope(new Scope),
		constants(new std::vector<std::shared_ptr<MathObj>>()),
		variables(new std::vector<std::shared_ptr<Variable>>()),
		functions(new std::vector<std::shared_ptr<Function>>()),
		operators(new std::vector<std::pair<std::shared_ptr<const OperatorFunction>, std::string>>())
	{}

	std::shared_ptr<std::vector<std::shared_ptr<MathObj>>> constants;
	std::shared_ptr<std::vector<std::shared_ptr<Variable>>> variables;
	std::shared_ptr<std::vector<std::shared_ptr<Function>>> functions;
	std::shared_ptr<std::vector<std::pair<std::shared_ptr<const OperatorFunction>, std::string>>> operators;

	void interpret_source(std::string_view source);
	void run(void);
};

#endif // VM_H