#include "compiler/compiler.hpp"
#include "global/globals.hpp"

void Compiler::compile_source(std::string_view source)
{
	if (!parser)
	{
		parser = std::make_unique<Parser>();
	}
	parser->parse_source(source);

	if (!semantic_analyzer)
	{
		semantic_analyzer = std::make_unique<SemanticAnalyzer>();
	}
	semantic_analyzer->analyze(parser->get_ast());

	auto & ast = parser->get_ast();
	for (const auto &statement : ast.statements)
	{
		compile_statement(statement.get());
	}
	chunk.emit(OP_RETURN);

	//* Debugging
	chunk.disassemble();
}

void Compiler::compile_statement(const ASTNode * statement_n)
{
	switch (statement_n->type)
	{
	case ASTNode::Type::N_EXPRESSION:
		{
			auto expression_statement = static_cast<const ExpressionNode *>(statement_n);
			compile_expression(expression_statement);
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

	switch (literal_n->type)
	{
	case MathObj::Type::MO_INTEGER:
		{
			int value = std::stoi(literal_n->value.data());
			chunk.constant_pool.add_constant(std::make_shared<IntegerObj>(value));
			chunk.emit_constant(chunk.constant_pool.size() - 1);
		}
		break;
	case MathObj::Type::MO_REAL:
		{
			double value = std::stod(literal_n->value.data());
			chunk.constant_pool.add_constant(std::make_shared<RealObj>(value));
			chunk.emit_constant(chunk.constant_pool.size() - 1);
		}

	default:
		break;
	}
}