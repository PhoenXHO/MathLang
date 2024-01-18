#ifndef SEMANALYZER_H
#define SEMANALYZER_H

#include <string>
#include <memory>
#include <stack>

#include "ast.h"
#include "mathobj.h"
#include "scope.h"

class SemanticAnalyzer
{
private:
	struct AnalysisResult {
		MathObjType type;
		ASTNode * node;

		AnalysisResult(MathObjType type) :
			type(type),
			node(nullptr)
		{}
		AnalysisResult(MathObjType type, ASTNode * node) :
			type(type),
			node(node)
		{}
	};

public:
	enum class ContextType
	{
		C_RETURNING_FUNCTION, C_NONRETURNING_FUNCTION, C_GLOBAL
	};
	
	std::shared_ptr<Scope> scope;
	
	SemanticAnalyzer(
		const AST & ast,
		std::unique_ptr<OperatorTable> operator_table,
		std::shared_ptr<Scope> & scope
	) :
		ast(ast),
		operator_table(std::move(operator_table)),
		scope(scope)
	{}

	void analyze_source(void);
	AnalysisResult analyze(ASTNode * node);

	bool in_function(void) const;

private:
	const AST & ast;
	const std::unique_ptr<OperatorTable> operator_table;

	bool panic_mode = false;
	std::stack<std::pair<ContextType, const ASTNode *>> context_stack;

	void register_semantic_error(std::string message, std::string additional_info, const ASTNode * node);

};
typedef SemanticAnalyzer::ContextType ContextType;

#endif // SEMANALYZER_H