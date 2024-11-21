#include "semantic_analyzer/semantic_analyzer.hpp"
#include "global/globals.hpp"

void SemanticAnalyzer::analyze(const AST & ast)
{
	for (auto & statement : ast.statements)
	{
		analyze(statement.get());
	}
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze(ASTNode * node)
{
	switch (node->type)
	{
	case ASTNode::Type::N_EXPRESSION:
		return analyze_expression(static_cast<ExpressionNode *>(node));
	case ASTNode::Type::N_OPERAND:
		return analyze_operand(static_cast<OperandNode *>(node));
	case ASTNode::Type::N_LITERAL:
		return analyze_literal(static_cast<LiteralNode *>(node));
	default:
		// This should never happen
		globals::error_handler.log_semantic_error("Unknown AST node type", 1, 1, 1, true);
	}

	return {MathObj::Type::MO_NONE};
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_expression(ExpressionNode * expression)
{
	AnalysisResult left = analyze(expression->left.get());
	AnalysisResult right = analyze(expression->right.get());

	// Find the candidate operator implementation for the given operands
	// If a candidate is found, return the result of the operation
	// If no candidate is found, log an error
	auto implentations = expression->op->get_implementations();
	//! For now, there is are no implicit conversions
	auto it = implentations.find({left.type, right.type});
	if (it != implentations.end())
	{
		// Store the implementation for the compiler
		expression->op->implementation = it->second;
		return it->second->get_result_type();
	}
	else
	{
		globals::error_handler.log_semantic_error("No operator implementation found for the given operands", 1, 1, 1, true);
	}

	return {MathObj::Type::MO_NONE};
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_operand(OperandNode * operand)
{
	AnalysisResult primary = analyze(operand->primary.get());

	if (!operand->op)
	{
		return primary;
	}

	// Find the candidate operator implementation for the given operands
	// If a candidate is found, return the result of the operation
	// If no candidate is found, log an error
	auto implentations = operand->op->get_implementations();
	//! For now, there is are no implicit conversions
	auto it = implentations.find({primary.type, MathObj::Type::MO_NONE});
	if (it != implentations.end())
	{
		// Store the implementation for the compiler
		operand->op->implementation = it->second;
		return it->second->get_result_type();
	}
	else
	{
		globals::error_handler.log_semantic_error("No operator implementation found for the given operand", 1, 1, 1, true);
	}

	return {MathObj::Type::MO_NONE};
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_literal(LiteralNode * literal)
{
	return literal->type;
}