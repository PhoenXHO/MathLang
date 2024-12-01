#include "compiler/compiler.hpp"
#include "object/object.hpp"
#include "global/globals.hpp"
#include "global/config.hpp"

void Compiler::compile_source(std::string_view source)
{
	parser->parse_source(source);
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
	case ASTNode::Type::N_EXPRESSION_STATEMENT:
		{
			auto expression_statement = static_cast<const ExpressionStatementNode *>(statement_n);
			compile_expression(expression_statement->expression.get());
			if (expression_statement->print_expression)
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
		break;

	case ASTNode::Type::N_OPERAND:
		{
			auto operand = static_cast<const OperandNode *>(statement_n);
			compile_operand(operand);
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
		globals::error_handler.log_compiletime_error(
			"Maximum number of operators reached",
			0, 0, 0, //TODO add line, column, position to AST nodes
			true
		);
		return;
	}

	operator_stack.push_back(implementation);
	chunk.emit(is_unary ? OP_CALL_UNARY : OP_CALL_BINARY, operator_stack.size() - 1);
}

void Compiler::compile_unary_operator(const OperatorNode * op)
{ compile_operator(op, true); }

void Compiler::compile_binary_operator(const OperatorNode * op)
{ compile_operator(op); }

void Compiler::compile_literal(const LiteralNode * literal_n)
{
	if (chunk.constant_pool.size() >= UINT8_MAX)
	{
		// We have reached the maximum number of constants
		globals::error_handler.log_compiletime_error(
			"Maximum number of constants reached",
			0, 0, 0, //TODO add line, column, position to AST nodes
			true
		);
		return;
	}

	try
	{
		switch (literal_n->type)
		{
		case MathObj::Type::MO_INTEGER:
			{
				std::string value_str(literal_n->value);
				mpz value(value_str);
				chunk.constant_pool.add_constant(std::make_shared<IntegerObj>(value));
				chunk.emit_constant(chunk.constant_pool.size() - 1);
			}
			break;
		case MathObj::Type::MO_REAL:
			{
				std::string value_str(literal_n->value);
				size_t precision_bits = value_str.size() * 8; // Rough estimate of the precision: 8 bits per digit
				mpfr value(value_str, precision_bits);
				chunk.constant_pool.add_constant(std::make_shared<RealObj>(value));
				chunk.emit_constant(chunk.constant_pool.size() - 1);
			}

		default:
			break;
		}
	}
	catch (const std::exception & e)
	{
		globals::error_handler.log_compiletime_error(
			"Exception occurred while compiling literal: " + std::string(e.what()),
			0, 0, 0, //TODO add line, column, position to AST nodes
			true
		);
	}
}