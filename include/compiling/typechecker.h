#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include <string>
#include <memory>

#include "ast.h"
#include "mathobj.h"
#include "scope.h"

class TypeChecker
{
	const AST & ast;
	const std::unique_ptr<OperatorTable> operator_table;

	bool panic_mode = false;

	void register_semantic_error(std::string message, std::string additional_info, const ASTNode * node);

public:
	std::shared_ptr<Scope> scope;
	
	TypeChecker(
		const AST & ast,
		std::unique_ptr<OperatorTable> operator_table,
		std::shared_ptr<Scope> & scope
	) :
		ast(ast),
		operator_table(std::move(operator_table)),
		scope(scope)
	{}

	void check_types(void);
	MathObjType check_types(const std::unique_ptr<ASTNode> & node);
};

#endif // TYPECHECKER_H