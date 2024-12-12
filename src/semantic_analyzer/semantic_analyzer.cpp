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
	case ASTNode::Type::N_FUNCTION_CALL:
		return analyze_function_call(static_cast<FunctionCallNode *>(node));
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
		// Log a warning instead of an error, to allow overwriting variables
		globals::error_handler.log_warning({
			"Variable '" + std::string(variable_declaration->identifier->name) + "' is already defined",
			variable_declaration->identifier->location,
			variable_declaration->identifier->length
		});
	}

	ClassPtr type = nullptr;
	// Analyze the type if it exists
	if (variable_declaration->type)
	{
		type = analyze_type(variable_declaration->type.get()).cls;
	}

	// Analyze the expression if it exists
	if (variable_declaration->expression)
	{
		//! For now, there is no casting in variable declarations
		//TODO: Implement constructors in classes
		auto result = analyze(variable_declaration->expression.get());
		if (type && !type->is_sub_class(result.cls))
		{
			globals::error_handler.log_semantic_error({
				"Type mismatch in variable declaration",
				variable_declaration->expression->location,
				variable_declaration->expression->length,
				"Expected: " + type->to_string() + '\n' +
				"Got: " + result.cls->to_string()
			}, true);
		}
	}

	// Add the variable to the current scope
	if (type)
	{
		current_scope->define_variable(variable_declaration->identifier->name, type);
	}
	else
	{
		// If the type is not defined, use the default type
		current_scope->define_variable(variable_declaration->identifier->name);
	}

	return {Builtins::none_class};
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_expression(ExpressionNode * expression)
{
	AnalysisResult left = analyze(expression->left.get());
	AnalysisResult right = analyze(expression->right.get());

	//TODO: Put this in a separate method
	// Find the candidate operator implementation for the given operands
	// If a candidate is found, return the result of the operation
	// If no candidate is found, log an error
	auto implentations = expression->op->implementations();
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

	//TODO: Put this in a separate method
	// Find the candidate operator implementation for the given operands
	// If a candidate is found, return the result of the operation
	// If no candidate is found, log an error
	auto implentations = operand->op->implementations();
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
			"Got: " + primary.cls->to_string()
		}, true);
	}

	return {Builtins::none_class};
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_function_call(FunctionCallNode * function_call)
{
	// Check if the function is defined in the current scope
	auto function = current_scope->find_function(function_call->identifier->name);
	if (!function.second)
	{
		globals::error_handler.log_semantic_error({
			"Function '" + std::string(function_call->identifier->name) + "' is not defined",
			function_call->identifier->location,
			function_call->identifier->length
		}, true);
	}
	
	// Analyze the arguments
	FunctionSignature signature;
	for (auto & argument : function_call->arguments)
	{
		auto result = analyze(argument.get());
		signature.parameters.push_back({ "", result.cls });
	}

	// Find the most specific implementation of the function for the given signature
	auto impl = function.second->find_implentation(signature);
	if (!impl.second)
	{
		globals::error_handler.log_semantic_error({
			"No function implementation found for the given arguments",
			function_call->location,
			function_call->length,
			"Arguments: " + signature.to_string(true)
		}, true);
	}

	// Store the index of the function for the compiler
	function_call->function_index = function.first;
	function_call->function_implementation_index = impl.first;
	return impl.second->return_class();
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

	// Get the type of the variable
	if (variable->value_class() == Builtins::none_class)
	{
		// The variable is not initialized
		globals::error_handler.log_semantic_error({
			"Variable '" + std::string(identifier->name) + "' is not initialized",
			identifier->location,
			identifier->length
		}, true);
	}

	return variable->get_class();
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_type(TypeNode * type)
{
	// Check if the class is defined
	auto cls = current_scope->get_classes()[type->name];
	if (!cls.second)
	{
		globals::error_handler.log_semantic_error({
			"Class '" + std::string(type->name) + "' is not defined",
			type->location,
			type->length
		}, true);
	}

	return cls.second;
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze_literal(LiteralNode * literal)
{
	return literal->cls;
}