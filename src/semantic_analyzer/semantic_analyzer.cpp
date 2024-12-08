#include "semantic_analyzer/semantic_analyzer.hpp"
#include "util/globals.hpp"
#include "class/builtins.hpp"

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
	case ASTNode::Type::N_VARIABLE_DECLARATION:
		return analyze_variable_declaration(static_cast<VariableDeclarationNode *>(node));
	case ASTNode::Type::N_EXPRESSION_STATEMENT:
		return analyze(static_cast<ExpressionStatementNode *>(node)->expression.get());
	case ASTNode::Type::N_EXPRESSION:
		return analyze_expression(static_cast<ExpressionNode *>(node));
	case ASTNode::Type::N_OPERAND:
		return analyze_operand(static_cast<OperandNode *>(node));
	case ASTNode::Type::N_IDENTIFIER:
		return analyze_identifier(static_cast<IdentifierNode *>(node));
	case ASTNode::Type::N_LITERAL:
		return analyze_literal(static_cast<LiteralNode *>(node));
	default:
		globals::error_handler.log_semantic_error({
			"Unknown AST node type",
			node->location,
			node->length
		}, true);
	}

	return {Builtins::none_class};
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_variable_declaration(VariableDeclarationNode * variable_declaration)
{
	// Check if the variable is already defined in the current scope
	if (current_scope->is_variable_defined(variable_declaration->identifier->name))
	{
		globals::error_handler.log_semantic_error({
			"Variable '" + std::string(variable_declaration->identifier->name) + "' is already defined",
			variable_declaration->identifier->location,
			variable_declaration->identifier->length
		}, true);
	}

	// Add the variable to the current scope
	current_scope->define_variable(variable_declaration->identifier->name);

	// Analyze the expression if it exists
	if (variable_declaration->expression)
	{
		return analyze(variable_declaration->expression.get());
	}

	return {Builtins::none_class};
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_expression(ExpressionNode * expression)
{
	AnalysisResult left = analyze(expression->left.get());
	AnalysisResult right = analyze(expression->right.get());

	// Find the candidate operator implementation for the given operands
	// If a candidate is found, return the result of the operation
	// If no candidate is found, log an error
	auto implentations = expression->op->get_implementations();
	auto impl = implentations.find_most_specific(left.cls, right.cls);
	if (impl)
	{
		// Store the implementation for the compiler
		expression->op->implementation = impl;
		return impl->result_class();
	}
	else
	{
		globals::error_handler.log_semantic_error({
			"No operator implementation found for the given operands",
			expression->op->location,
			expression->op->length,
			"",
			"Got: " + left.cls->to_string() + " and " + right.cls->to_string()
		}, true);
	}

	return {Builtins::none_class};
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
	auto impl = implentations.find_most_specific(primary.cls, Builtins::none_class);
	if (impl)
	{
		// Store the implementation for the compiler
		operand->op->implementation = impl;
		return impl->result_class();
	}
	else
	{
		globals::error_handler.log_semantic_error({
			"No operator implementation found for the given operand",
			operand->op->location,
			operand->op->length,
			"",
			"Got: " + primary.cls->to_string()
		}, true);
	}

	return {Builtins::none_class};
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_identifier(IdentifierNode * identifier)
{
	// Check if the variable is defined in the current scope
	auto & variable = current_scope->get_variable(identifier->name);
	if (!variable)
	{
		globals::error_handler.log_semantic_error({
			"Variable '" + std::string(identifier->name) + "' is not defined",
			identifier->location,
			identifier->length
		}, true);
	}

	//// Get the type of the variable
	//if (!variable->value())
	//{
	//	// The variable is not initialized
	//	globals::error_handler.log_semantic_error(
	//		"Variable '" + std::string(identifier->name) + "' is not initialized",
	//		identifier->location,
	//		identifier->length,
	//		true
	//	);
	//}

	return variable->value_class();
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_literal(LiteralNode * literal)
{
	return literal->cls;
}