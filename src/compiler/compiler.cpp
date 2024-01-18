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
		register_compile_error("too many scopes", "", block_n);
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

void Compiler::compile_return_statement(const ReturnStatementNode * return_statement_n)
{
	compile_expression(return_statement_n->value.get());
	emit(OP_RETURN_VALUE);
}

void Compiler::compile_function_declaration(const FunctionDeclarationNode * func_decl_n)
{
	if (functions->size() >= UINT8_MAX)
	{
		register_compile_error("too many functions", "", func_decl_n);
		return;
	}

	uint8_t arg = functions->size();
	auto function = scope->find_function(func_decl_n->name->name);
	functions->push_back(function->second);
	scope->function_indices[std::string(func_decl_n->name->name)] = arg;

	function->second->chunk = std::make_shared<Chunk>(func_decl_n->name->name);
	function->second->chunk->parent = chunk;
	chunk = function->second->chunk;

	enter_function(scope, function->second);
	for (auto it = func_decl_n->parameters.rbegin(); it != func_decl_n->parameters.rend(); ++it)
	{
		compile_parameter(it->get());
	}

	for (auto & statement_n : func_decl_n->body->statements)
	{
		compile_statement(statement_n.get());
	}
	emit(OP_LEAVE_FUNCTION);
	leave_scope(scope);

	chunk = chunk->parent;
}

void Compiler::compile_function_call(const FunctionCallNode * func_call_n)
{
	auto function = scope->find_function(func_call_n->name->name);
	uint8_t arg = scope->find_function_index(std::string(func_call_n->name->name));

	for (auto & arg_n : func_call_n->arguments)
	{
		compile_expression(arg_n.get());
	}

	emit(OP_CALL_FUNCTION, arg);
}

void Compiler::compile_parameter(const ParameterNode * parameter_n)
{
	if (variables->size() >= UINT8_MAX)
	{
		register_compile_error("too many variables", "", parameter_n);
		return;
	}

	uint8_t arg = variables->size();
	auto variable = scope->find_variable(parameter_n->name->name);
	variables->push_back(variable->second);
	scope->variable_indices[std::string(parameter_n->name->name)] = arg;

	if (parameter_n->default_value)
	{
		// TODO: for later
	}
	else
	{
		emit(OP_SET_VAR, arg);
	}
}

void Compiler::compile_statement(const ASTNode * statement_n)
{
	switch (statement_n->n_type)
	{
		case NodeType::N_BLOCK:
		{
			auto block_n = dynamic_cast<const BlockNode *>(statement_n);
			if (!block_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_statement()`");

			compile_block(block_n);
			break;
		}
		case NodeType::N_FUNC_DECL:
		{
			auto func_decl_n = dynamic_cast<const FunctionDeclarationNode *>(statement_n);
			if (!func_decl_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_statement()`");

			compile_function_declaration(func_decl_n);
			break;
		}
		case NodeType::N_EXPR_STMT:
		{
			auto expr_stmt_n = dynamic_cast<const ExpressionStatementNode *>(statement_n);
			if (!expr_stmt_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_statement()`");

			for (auto &expression_n : expr_stmt_n->expressions)
			{
				compile_expression(expression_n.get());
				emit(OP_POP);
			}
			break;
		}
		case NodeType::N_VAR_DECL:
		{
			auto var_decl_n = dynamic_cast<const VariableDeclarationNode *>(statement_n);
			if (!var_decl_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_statement()`");

			compile_variable_declaration(var_decl_n);
			break;
		}
		case NodeType::N_RETURN_STMT:
		{
			auto return_stmt_n = dynamic_cast<const ReturnStatementNode *>(statement_n);
			if (!return_stmt_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_statement()`");

			compile_return_statement(return_stmt_n);
			break;
		}
		case NodeType::N_RETURN:
			emit(OP_RETURN);
			break;
		default:
			throw std::runtime_error("unknown statement type");
	}
}

void Compiler::compile_variable_declaration(const VariableDeclarationNode * var_decl_n)
{
	if (variables->size() >= UINT8_MAX)
	{
		register_compile_error("too many variables", "", var_decl_n);
		return;
	}

	uint8_t arg = variables->size();
	auto variable = scope->find_variable(var_decl_n->name->name);
	variables->push_back(variable->second);
	scope->variable_indices[std::string(var_decl_n->name->name)] = arg;

	if (var_decl_n->value)
	{
		compile_expression(var_decl_n->value.get());
		emit(OP_SET_VAR, arg);
	}
}

void Compiler::compile_expression(const ASTNode * expression_n)
{
	switch (expression_n->n_type)
	{
		case NodeType::N_OPERAND:
		{
			auto operand_n = dynamic_cast<const OperandNode *>(expression_n);
			if (!operand_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_expression()`");
			compile_operand(operand_n);
			break;
		}
		case NodeType::N_EXPR:
		{
			auto expr_n = dynamic_cast<const ExpressionNode *>(expression_n);
			if (!expr_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_expression()`");
			compile_expression(expr_n->left.get());
			compile_expression(expr_n->right.get());
			compile_binary_operator(expr_n->op.get());
			break;
		}
		case NodeType::N_IDENTIFIER:
		{
			auto identifier_n = dynamic_cast<const IdentifierNode *>(expression_n);
			if (!identifier_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_expression()`");
			compile_identifier(identifier_n);
			break;
		}
		case NodeType::N_LITERAL:
		{
			auto literal_n = dynamic_cast<const LiteralNode *>(expression_n);
			if (!literal_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_expression()`");
			compile_literal(literal_n);
			break;
		}
		case NodeType::N_FUNC_CALL:
		{
			auto func_call_n = dynamic_cast<const FunctionCallNode *>(expression_n);
			if (!func_call_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_expression()`");
			compile_function_call(func_call_n);
			break;
		}
		default:
			throw std::runtime_error("unknown expression type");
	}
}

void Compiler::compile_operand(const OperandNode * operand_n)
{
	switch (operand_n->primary->n_type)
	{
		case NodeType::N_EXPR:
		{
			compile_expression(operand_n->primary.get());
			break;
		}
		case NodeType::N_OPERAND:
		{
			auto _operand_n = dynamic_cast<const OperandNode *>(operand_n->primary.get());
			if (!_operand_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_operand()`");
			compile_operand(_operand_n);
			break;
		}
		case NodeType::N_IDENTIFIER:
		{
			auto identifier_n = dynamic_cast<IdentifierNode *>(operand_n->primary.get());
			if (!identifier_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_operand()`");
			compile_identifier(identifier_n);
			break;
		}
		case NodeType::N_LITERAL:
		{
			auto literal_n = dynamic_cast<LiteralNode *>(operand_n->primary.get());
			if (!literal_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_operand()`");
			compile_literal(literal_n);
			break;
		}
		case NodeType::N_FUNC_CALL:
		{
			auto func_call_n = dynamic_cast<FunctionCallNode *>(operand_n->primary.get());
			if (!func_call_n)
				throw std::logic_error("downcasting failed in `Compiler::compile_operand()`");
			compile_function_call(func_call_n);
			break;
		}
		default:
			throw std::logic_error("Invalid node type in `Compiler::compile_operand()`");
	}

	if (operand_n->op)
		compile_unary_operator(operand_n->op.get());
}

void Compiler::compile_operator(const OperatorNode * operator_n, bool unary)
{
	auto op_func = operator_n->op_func;
	if (operators->size() >= UINT8_MAX)
		throw std::runtime_error("too many operators");

	auto pair = std::make_pair(op_func, operator_n->op_info->name);
	operators->push_back(pair);

	emit(unary ? OP_UNARY_OP : OP_BINARY_OP, operators->size() - 1);
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
	if (chunk->constants.size() >= UINT8_MAX)
	{
		throw std::runtime_error("too many constants");
	}

	switch (literal_n->type)
	{
		case MathObjType::MO_INTEGER:
		{
			auto constant = std::make_shared<Integer>(std::stoi(literal_n->value.data()));
			chunk->constants.push_back(constant);
			emit(OP_LOAD_CONST, chunk->constants.size() - 1);
			break;
		}
		case MathObjType::MO_REAL:
		{
			auto constant = std::make_shared<Real>(std::stod(literal_n->value.data()));
			chunk->constants.push_back(constant);
			emit(OP_LOAD_CONST, chunk->constants.size() - 1);
			break;
		}
	}
}

void Compiler::emit(uint8_t op_code)
{
	chunk->bytes.push_back(op_code);
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