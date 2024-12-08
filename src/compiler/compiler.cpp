#include <any>

#include "compiler/compiler.hpp"
#include "object/integer_object.hpp"
#include "object/real_object.hpp"
#include "util/globals.hpp"
#include "util/config.hpp"
#include "class/builtins.hpp"

void Compiler::compile_source(void)
{
	parser->parse_source();
	semantic_analyzer->analyze(parser->get_ast());

	auto & ast = parser->get_ast();
	for (const auto &statement : ast.statements)
	{
		compile_statement(statement.get());
	}

	if (!config::print_all && last_print != 0)
	{
		chunk.emit_at(last_print, OP_PRINT);
	}

	chunk.emit(OP_RETURN);

	//* Debugging
	chunk.disassemble();
}

void Compiler::compile_statement(const ASTNode * statement_n)
{
	switch (statement_n->type)
	{
	case ASTNode::Type::N_VARIABLE_DECLARATION:
		{
			auto variable_declaration = static_cast<const VariableDeclarationNode *>(statement_n);
			compile_variable_declaration(variable_declaration);
		}
		break;

	case ASTNode::Type::N_EXPRESSION_STATEMENT:
		{
			auto expression_statement = static_cast<const ExpressionStatementNode *>(statement_n);
			compile_expression(expression_statement->expression.get());
			compile_print(expression_statement->print_expression);
		}
		break;

	case ASTNode::Type::N_OPERAND:
		{
			auto operand = static_cast<const OperandNode *>(statement_n);
			compile_operand(operand);
		}
		break;

	case ASTNode::Type::N_IDENTIFIER:
		{
			auto identifier = static_cast<const IdentifierNode *>(statement_n);
			compile_identifier(identifier);
		}
		break;
	
	case ASTNode::Type::N_LITERAL:
		{
			auto literal = static_cast<const LiteralNode *>(statement_n);
			compile_literal(literal);
		}
		break;

	default:
		break;
	}
}

void Compiler::compile_variable_declaration(const VariableDeclarationNode * variable_declaration)
{
	// The semantic analyzer should have already added the variable to the current scope
	size_t index = current_scope->get_variable_index(variable_declaration->identifier->name);
	if (index >= UINT8_MAX)
	{
		// We have reached the maximum number of variables
		globals::error_handler.log_compiletime_error({
			"Maximum number of variables reached",
			variable_declaration->identifier->location,
			variable_declaration->identifier->length
		}, true);
		return;
	}

	// Compile the expression if it exists
	if (variable_declaration->expression)
	{
		compile_expression(variable_declaration->expression.get());
		chunk.emit(OP_SET_VARIABLE, index); // Set the variable in the current scope to the value on the stack and print it
		compile_print(variable_declaration->print_expression);
	}
}

void Compiler::compile_expression(const ASTNode * expression_n)
{
	switch (expression_n->type)
	{
	case ASTNode::Type::N_OPERAND:
		{
			auto operand = static_cast<const OperandNode *>(expression_n);
			compile_operand(operand);
		}
		break;
	case ASTNode::Type::N_EXPRESSION:
		{
			auto expression = static_cast<const ExpressionNode *>(expression_n);
			compile_expression(expression->left.get());
			compile_expression(expression->right.get());
			compile_binary_operator(expression->op.get());
		}
		break;
	case ASTNode::Type::N_IDENTIFIER:
		{
			auto identifier = static_cast<const IdentifierNode *>(expression_n);
			compile_identifier(identifier);
		}
		break;
	case ASTNode::Type::N_LITERAL:
		{
			auto literal = static_cast<const LiteralNode *>(expression_n);
			compile_literal(literal);
		}
		break;

	default:
		break;
	}
}

void Compiler::compile_operand(const OperandNode * operand_n)
{
	switch (operand_n->primary->type)
	{
	case ASTNode::Type::N_EXPRESSION:
		{
			compile_expression(operand_n->primary.get());
		}
		break;
	case ASTNode::Type::N_OPERAND:
		{
			auto operand = static_cast<const OperandNode *>(operand_n->primary.get());
			compile_operand(operand);
		}
		break;
	case ASTNode::Type::N_LITERAL:
		{
			auto literal = static_cast<const LiteralNode *>(operand_n->primary.get());
			compile_literal(literal);
		}
		break;
	}

	// Compile the unary operator if it exists after the primary operand has been compiled
	if (operand_n->op)
	{
		compile_unary_operator(operand_n->op.get());
	}
}

void Compiler::compile_operator(const OperatorNode * op, bool is_unary)
{
	auto & implementation = op->implementation;
	if (operator_stack.size() >= UINT8_MAX)
	{
		// We have reached the maximum number of operators
		globals::error_handler.log_compiletime_error({
			"Maximum number of operators reached",
			op->location,
			op->length
		}, true);
		return;
	}

	operator_stack.push_back(implementation);
	chunk.emit(is_unary ? OP_CALL_UNARY : OP_CALL_BINARY, operator_stack.size() - 1);
}

void Compiler::compile_unary_operator(const OperatorNode * op)
{ compile_operator(op, true); }

void Compiler::compile_binary_operator(const OperatorNode * op)
{ compile_operator(op); }

void Compiler::compile_identifier(const IdentifierNode * identifier_n)
{
	size_t index = current_scope->get_variable_index(identifier_n->name);
	if (index >= UINT8_MAX)
	{
		// We have reached the maximum number of variables
		globals::error_handler.log_compiletime_error({
			"Maximum number of variables reached",
			identifier_n->location,
			identifier_n->length
		}, true);
		return;
	}

	chunk.emit(OP_GET_VARIABLE, index);
}

void Compiler::compile_literal(const LiteralNode * literal_n)
{
	if (constant_pool.size() >= UINT8_MAX)
	{
		// We have reached the maximum number of constants
		globals::error_handler.log_compiletime_error({
			"Maximum number of constants reached",
			literal_n->location,
			literal_n->length
		}, true);
		return;
	}

	try
	{
		auto value = literal_n->cls->instantiate(std::any(literal_n->value));
		constant_pool.add_constant(value);
		chunk.emit_constant(constant_pool.size() - 1);
	}
	catch (const std::exception & e)
	{
		globals::error_handler.log_compiletime_error({
			"Exception occurred while compiling literal: " + std::string(e.what()),
			literal_n->location,
			literal_n->length
		}, true);
	}
}

void Compiler::compile_print(bool print)
{
	if (print)
	{
		if (config::print_all)
		{
			chunk.emit(OP_PRINT);
		}
		else
		{
			// Print only the last expression if the print_all flag is not set
			last_print = chunk.code.size();
			chunk.emit(OP_POP);
		}
	}
	else
	{
		chunk.emit(OP_POP);
	}
}