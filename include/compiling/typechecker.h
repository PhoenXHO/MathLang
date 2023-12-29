#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include <string>
#include <memory>
#include <tuple>

#include "ast.h"
#include "mathobj.h"

class TypeChecker
{
	const AST & ast;
	const std::unique_ptr<OperatorTable> operator_table;
	std::unordered_map<std::string_view, std::tuple<MathObjType, std::shared_ptr<MathObj>>> variables;

	bool panic_mode = false;

	void register_semantic_error(std::string message, std::string additional_info, const ASTNode * node);

public:
	TypeChecker(const AST & ast, std::unique_ptr<OperatorTable> operator_table) :
		ast(ast),
		operator_table(std::move(operator_table))
	{}

	void check_types(void);
	MathObjType check_types(const std::unique_ptr<ASTNode> & node);
};

#endif // TYPECHECKER_H