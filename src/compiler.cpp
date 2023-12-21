#include "compiler.h"

std::shared_ptr<Compiler> compiler;

void Compiler::compile_source(void)
{
	for (auto & statement_n : ast.statements)
	{
		compile_statement(statement_n.get());
	}
	emit(OP_RETURN);
}

void Compiler::compile_statement(const ASTNode * statement_n)
{
	// Expression statement
	if (statement_n->n_type == NodeType::N_EXPR_STMT)
	{
		auto expr_stmt_n = dynamic_cast<const ExpressionStatementNode *>(statement_n);

		if (!expr_stmt_n)
			throw std::logic_error("downcasting failed in `Compiler::compile_statement()`");

		for (auto & expression_n : expr_stmt_n->expressions)
		{
			compile_expression(expression_n.get());
			emit(OP_POP);
		}
	}
	// Variable declaration
	else if (statement_n->n_type == NodeType::N_VAR_DECL)
	{

	}
}

void Compiler::compile_expression(const ASTNode * expression_n)
{
	// Operand
	if (expression_n->n_type == NodeType::N_OPERAND)
	{
		auto operand_n = dynamic_cast<const OperandNode *>(expression_n);

		if (!operand_n)
			throw std::logic_error("downcasting failed in `Compiler::compile_expression()`");
		
		compile_operand(operand_n);
	}
	// Expression
	else
	{
		auto expr_n = dynamic_cast<const ExpressionNode *>(expression_n);

		if (!expr_n)
			throw std::logic_error("downcasting failed in `Compiler::compile_expression()`");

		compile_expression(expr_n->left.get());
		compile_expression(expr_n->right.get());
		compile_binary_operator(expr_n->op.get());
	}
}

void Compiler::compile_operand(const OperandNode * operand_n)
{
	// Expression
	if (operand_n->primary->n_type == NodeType::N_EXPR)
	{
		compile_expression(operand_n->primary.get());
	}
	// Operand
	else if (operand_n->primary->n_type == NodeType::N_OPERAND)
	{
		auto _operand_n = dynamic_cast<const OperandNode *>(
			operand_n->primary.get()
		);

		if (!_operand_n)
			throw std::logic_error("downcasting failed in `Compiler::compile_operand()`");

		compile_operand(_operand_n);
	}
	// Identifier
	else if (operand_n->primary->n_type == NodeType::N_IDENTIFIER)
	{
		auto identifier_n = dynamic_cast<IdentifierNode *>(
			operand_n->primary.get()
		);

		if (!identifier_n)
			throw std::logic_error("downcasting failed in `Compiler::compile_operand()`");

		compile_identifier(identifier_n);
	}
	// Literal
	else if (operand_n->primary->n_type == NodeType::N_LITERAL)
	{
		auto literal_n = dynamic_cast<LiteralNode *>(
			operand_n->primary.get()
		);

		if (!literal_n)
			throw std::logic_error("downcasting failed in `Compiler::compile_operand()`");

		compile_literal(literal_n);
	}

	if (operand_n->op)
		compile_unary_operator(operand_n->op.get());
}

void Compiler::compile_operator(const OperatorNode * operator_n, bool unary)
{
	auto * op_info = operator_table->get_opinfo(operator_n->op_sym, unary);
	if (!op_info)
		throw std::runtime_error("invalid operator `" + std::string(operator_n->op_sym) + '`');

	if (operators.size() >= UINT8_MAX)
		throw std::runtime_error("too many operators");

	uint8_t arg;
	switch (op_info->type)
	{
		case OperatorType::O_BUILTIN:
			auto * builtin_op_info = dynamic_cast<const BuiltinOperator *>(op_info);
			if (!builtin_op_info)
				throw std::logic_error("downcasting failed in `Compiler::compile_binary_operator()`");
			operators.push_back(builtin_op_info);
			arg = operators.size() - 1;
			break;
		//case OperatorType::O_CUSTOM:
		//	break;
	}

	emit(unary ? OP_UNARY_OP : OP_BINARY_OP, arg);
}

void Compiler::compile_binary_operator(const OperatorNode * operator_n)
{ compile_operator(operator_n, false); }

void Compiler::compile_unary_operator(const OperatorNode * operator_n)
{ compile_operator(operator_n, true); }

void Compiler::compile_identifier(const IdentifierNode * identifier_n)
{
	// TODO for later
}

void Compiler::compile_literal(const LiteralNode * literal_n)
{
	switch (literal_n->type)
	{
		case Type::T_INTEGER: case Type::T_REAL:
			compile_constant(literal_n);
			break;
	}
}

void Compiler::compile_constant(const LiteralNode * literal_n)
{
	if (constants.size() >= UINT8_MAX)
	{
		throw std::runtime_error("too many constants");
	}

	switch (literal_n->type)
	{
		case Type::T_INTEGER: case Type::T_REAL:
			double value = std::stod(literal_n->value.data());
			auto constant = std::make_shared<RealValue>(value);
			constants.push_back(constant);
			emit(OP_LOAD_CONST, constants.size() - 1);
			break;
	}
}

void Compiler::emit(uint8_t op_code)
{
	bytes.push_back(op_code);
}
void Compiler::emit(uint8_t op_code, uint8_t arg)
{
	emit(op_code);
	emit(arg);
}