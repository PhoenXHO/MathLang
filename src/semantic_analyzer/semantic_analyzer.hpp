#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include <memory>

#include "parser/ast.hpp"
#include "object/object.hpp"
#include "scope/scope.hpp"

// This class will be responsible for type checking and other semantic analysis tasks
class SemanticAnalyzer
{
	struct AnalysisResult
	{
		MathObj::Type type; // The type of the expression
		bool is_const; // Whether the expression is constant

		/**
		 * @brief Construct a new Analysis Result object
		 * @param type The type of the expression
		 * @param is_const Whether the expression is constant
		 **/
		AnalysisResult(MathObj::Type type, bool is_const = true) : type(type), is_const(is_const) {}
	};

	std::shared_ptr<Scope> global_scope;
	std::shared_ptr<Scope> current_scope = global_scope;

public:
	SemanticAnalyzer(std::shared_ptr<Scope> global_scope) : global_scope(global_scope) {}
	~SemanticAnalyzer() = default;

	void reset(void)
	{
		current_scope = global_scope;
	}

	void analyze(const AST & ast);

private:
	AnalysisResult analyze(ASTNode * node);
	AnalysisResult analyze_variable_declaration(VariableDeclarationNode * variable_declaration);
	AnalysisResult analyze_expression(ExpressionNode * expression);
	AnalysisResult analyze_operand(OperandNode * operand);
	AnalysisResult analyze_identifier(IdentifierNode * identifier);
	AnalysisResult analyze_literal(LiteralNode * literal);
};


#endif // SEMANTIC_ANALYZER_HPP