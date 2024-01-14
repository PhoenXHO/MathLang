#include "compiler.h"
#include "error.h"

void Compiler::compile_source(void)
{
	for (auto & statement_n : ast.statements)
	{
		compile_statement(statement_n.get());
	}
	emit(OP_RETURN);
}

void Compiler::compile_block(const BlockNode * block_n)
{
	if (scope->children.size() >= UINT8_MAX)
	{
		register_compile_error("too many blocks", "", block_n);
		return;
	}

	enter_scope(scope, block_n->relative_index);
	emit(OP_ENTER_BLOCK);
	for (auto & statement_n : block_n->statements)
	{
		compile_statement(statement_n.get());
	}
	leave_scope(scope);
	emit(OP_LEAVE_BLOCK);
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
		auto var_decl_n = dynamic_cast<const VariableDeclarationNode *>(statement_n);

		if (!var_decl_n)
			throw std::logic_error("downcasting failed in `Compiler::compile_statement()`");

		compile_variable_declaration(var_decl_n);
	}
	// Block
	else if (statement_n->n_type == NodeType::N_BLOCK)
	{
		auto block_n = dynamic_cast<const BlockNode *>(statement_n);

		if (!block_n)
			throw std::logic_error("downcasting failed in `Compiler::compile_statement()`");

		compile_block(block_n);
	}
}

void Compiler::compile_variable_declaration(const VariableDeclarationNode * var_decl_n)
{
	if (variables.size() >= UINT8_MAX)
	{
		register_compile_error("too many variables", "", var_decl_n);
		return;
	}

	uint8_t arg = variables.size();
	auto variable = scope->find_variable(var_decl_n->name->name);
	variables.push_back(variable->second);
	scope->variable_indices[std::string(var_decl_n->name->name)] = arg;

	if (var_decl_n->value)
	{
		compile_expression(var_decl_n->value.get());
		emit(OP_SET_VAR, arg);
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
	auto op_func = operator_n->op_func;
	if (operators.size() >= UINT8_MAX)
		throw std::runtime_error("too many operators");

	auto pair = std::make_pair(op_func, operator_n->op_info->name);
	operators.push_back(pair);

	emit(unary ? OP_UNARY_OP : OP_BINARY_OP, operators.size() - 1);
}

void Compiler::compile_binary_operator(const OperatorNode * operator_n)
{ compile_operator(operator_n, false); }

void Compiler::compile_unary_operator(const OperatorNode * operator_n)
{ compile_operator(operator_n, true); }

void Compiler::compile_identifier(const IdentifierNode * identifier_n)
{
	std::string name = std::string(identifier_n->name);
	uint8_t variable = scope->find_variable_index(name);
	emit(OP_LOAD_VAR, variable);
}

void Compiler::compile_literal(const LiteralNode * literal_n)
{
	switch (literal_n->type)
	{
		case MathObjType::MO_INTEGER: case MathObjType::MO_REAL:
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
		case MathObjType::MO_INTEGER:
		{
			auto constant = std::make_shared<Integer>(std::stoi(literal_n->value.data()));
			constants.push_back(constant);
			emit(OP_LOAD_CONST, constants.size() - 1);
			break;
		}
		case MathObjType::MO_REAL:
		{
			auto constant = std::make_shared<Real>(std::stod(literal_n->value.data()));
			constants.push_back(constant);
			emit(OP_LOAD_CONST, constants.size() - 1);
			break;
		}
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

void Compiler::register_compile_error(std::string message, std::string additional_info, const ASTNode * node)
{
	std::unique_ptr<Error> err { new CompileError(
		message,
		additional_info,
		node->line,
		node->column,
		node->start_position,
		node->end_position - node->start_position
	)};
	ErrorHandler::push_error(err);
}