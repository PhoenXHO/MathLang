#include "parser.h"
#include "globals.h"
#include "ast.h"
#include "error.h"
#include "debug.h"

std::shared_ptr<Parser> parser;

void Parser::parse_source(void)
{
	while (consume_tk())
	{
		ast
			.statements
			.push_back(statement_n());
		panic_mode = false;
	}
}

Parser::Parser(Lexer & lexer) : lexer(std::make_unique<Lexer>(lexer))
{
	panic_mode = false;

	operators = std::make_unique<OperatorTable>();
	operators->register_builtin_operators();
}

std::unique_ptr<ASTNode> Parser::statement_n(void)
{
	switch (curr_tk->type())
	{
		case TokenType::T_LET:
			return variable_declaration_n();

		case TokenType::T_LEFT_BRACE:
			return block_n();

		case TokenType::T_DEFINE:
			return function_declaration_n();

		case TokenType::T_RETURN:
			return return_n();

		case TokenType::T_COLON_ARROW:
			return return_statement_n();
		
		case TokenType::T_EOF:
			return nullptr;

		default:
			return expression_statement_n();
	}
}

std::unique_ptr<BlockNode> Parser::block_n(void)
{
	std::unique_ptr<BlockNode> block_node { new BlockNode };
	block_node->line = curr_tk->line();
	block_node->column = curr_tk->column();
	block_node->start_position = curr_tk->position();

	while (consume_tk() && curr_tk->type() != TokenType::T_RIGHT_BRACE && curr_tk->type() != TokenType::T_EOF)
	{
		block_node
			->statements
			.push_back(statement_n());
	}

	expect_tk(TokenType::T_RIGHT_BRACE, "`}` expected after block");

	block_node->end_position = curr_tk->position() + 1;
	return block_node;
}

std::unique_ptr<ReturnNode> Parser::return_n(void)
{
	std::unique_ptr<ReturnNode> return_node { new ReturnNode };
	return_node->line = curr_tk->line();
	return_node->column = curr_tk->column();
	return_node->start_position = curr_tk->position();
	return_node->end_position = curr_tk->position() + curr_tk->lexeme().length();
	consume_tk(); // consume `return`
	expect_tk(TokenType::T_SEMICOLON, "`;` expected after return statement");
	return return_node;
}

std::unique_ptr<ReturnStatementNode> Parser::return_statement_n(void)
{
	std::unique_ptr<ReturnStatementNode> return_stmt_node { new ReturnStatementNode };
	return_stmt_node->line = curr_tk->line();
	return_stmt_node->column = curr_tk->column();
	return_stmt_node->start_position = curr_tk->position();

	consume_tk(); // consume `:->`

	return_stmt_node->value = expression_n(P_MIN);
	if (!return_stmt_node->value)
	{
		register_syntax_error("expression expected after `:->`");
		return nullptr;
	}
	expect_tk(TokenType::T_SEMICOLON, "`;` expected after return statement");

	return_stmt_node->end_position = return_stmt_node->value->end_position;
	return return_stmt_node;
}

std::unique_ptr<FunctionDeclarationNode> Parser::function_declaration_n(void)
{
	std::unique_ptr<FunctionDeclarationNode> func_dec_node { new FunctionDeclarationNode };
	func_dec_node->line = curr_tk->line();
	func_dec_node->column = curr_tk->column();
	func_dec_node->start_position = curr_tk->position();

	consume_tk(); // consume `define`

	func_dec_node->name = identifier_n();
	if (!func_dec_node->name)
	{
		register_syntax_error("identifier expected after `define`");
		return nullptr;
	}
	consume_tk();

	expect_tk(TokenType::T_LEFT_PAREN, "`(` expected after function name");
	while (consume_tk()
		&& curr_tk->type() != TokenType::T_RIGHT_PAREN
		&& curr_tk->type() != TokenType::T_EOF)
	{
		func_dec_node
			->parameters
			.push_back(parameter_n());

		if (curr_tk->type() != TokenType::T_COMMA)
			break;
	}
	expect_tk(TokenType::T_RIGHT_PAREN, "`)` expected after function parameters");

	if (next_tk->type() == TokenType::T_ARROW)
	{
		consume_tk(); // consume `)`
		consume_tk(); // consume `->`
		func_dec_node->return_type = type_n();
		if (!func_dec_node->return_type)
		{
			register_syntax_error("return type expected after `->`");
			return nullptr;
		}
		func_dec_node->end_position = curr_tk->position() + curr_tk->lexeme().length();
		consume_tk();
	}
	else
	{
		func_dec_node->end_position = curr_tk->position() + 1;
		consume_tk(); // consume `)`
		func_dec_node->return_type = std::make_unique<TypeNode>(MathObjType(MOT::MO_NONE));
	}

	expect_tk(TokenType::T_LEFT_BRACE, "`{` expected after function declaration");

	func_dec_node->body = block_n();
	if (!func_dec_node->body)
	{
		register_syntax_error("function body expected");
		return nullptr;
	}

	return func_dec_node;
}

std::unique_ptr<FunctionCallNode> Parser::function_call_n(void)
{
	std::unique_ptr<FunctionCallNode> func_call_node { new FunctionCallNode };
	func_call_node->line = curr_tk->line();
	func_call_node->column = curr_tk->column();
	func_call_node->start_position = curr_tk->position();

	func_call_node->name = identifier_n();
	if (!func_call_node->name)
	{
		register_syntax_error("identifier expected as function name");
		return nullptr;
	}
	consume_tk();

	// just in case of a bug
	expect_tk(TokenType::T_LEFT_PAREN, "`(` expected after function name");
	while (consume_tk()
		&& curr_tk->type() != TokenType::T_RIGHT_PAREN
		&& curr_tk->type() != TokenType::T_EOF)
	{
		func_call_node
			->arguments
			.push_back(expression_n(P_MIN));
		panic_mode = false;

		if (curr_tk->type() != TokenType::T_COMMA)
			break;
	}
	expect_tk(TokenType::T_RIGHT_PAREN, "`)` expected after function arguments");

	func_call_node->end_position = curr_tk->position() + 1;
	return func_call_node;
}

std::unique_ptr<ParameterNode> Parser::parameter_n(void)
{
	std::unique_ptr<ParameterNode> param_node { new ParameterNode };
	param_node->line = curr_tk->line();
	param_node->column = curr_tk->column();
	param_node->start_position = curr_tk->position();

	param_node->name = identifier_n();
	if (!param_node->name)
	{
		register_syntax_error("identifier expected as parameter name");
		return nullptr;
	}
	consume_tk();

	expect_tk(TokenType::T_COLON, "`:` expected after parameter name");
	consume_tk(); // consume `:`
	param_node->type = type_n();
	if (!param_node->type)
	{
		register_syntax_error("type expected after `:`");
		return nullptr;
	}
	consume_tk();

	if (curr_tk->type() == TokenType::T_OPERATOR_SYM && curr_tk->lexeme() == "=")
	{
		consume_tk(); // consume `=`
		param_node->default_value = expression_n(P_MIN);
	}
	else
	{
		param_node->default_value = nullptr;
	}

	param_node->end_position = curr_tk->position() + 1;
	return param_node;
}

std::unique_ptr<VariableDeclarationNode> Parser::variable_declaration_n(void)
{
	consume_tk(); // consume `let`
	std::unique_ptr<VariableDeclarationNode> var_dec_node { new VariableDeclarationNode };
	var_dec_node->line = curr_tk->line();
	var_dec_node->start_position = curr_tk->position();

	var_dec_node->type = type_n();
	if (!var_dec_node->type || var_dec_node->type->type.type == MOT::MO_NONE)
	{
		register_syntax_error("type expected after `let`");
		return nullptr;
	}
	consume_tk();
	
	var_dec_node->name = identifier_n();
	if (!var_dec_node->name)
	{
		register_syntax_error("identifier expected after type");
		return nullptr;
	}
	consume_tk();

	if (curr_tk->type() == TokenType::T_COLON_EQUAL)
	{
		consume_tk(); // consume `:=`
		var_dec_node->value = expression_n(P_MIN);
		if (!var_dec_node->value)
		{
			register_syntax_error("expression expected after `:=`");
			return nullptr;
		}
	}
	else
	{
		var_dec_node->value = nullptr;
	}
	expect_tk(TokenType::T_SEMICOLON, "`;` expected after variable declaration");

	var_dec_node->column = var_dec_node->name->column;
	var_dec_node->end_position = curr_tk->position() + 1;
	return var_dec_node;
}

std::unique_ptr<ExpressionStatementNode> Parser::expression_statement_n(void)
{
	std::unique_ptr<ExpressionStatementNode> expr_stmt_node { new ExpressionStatementNode };

	auto expr_node = expression_n(P_MIN);
	if (!expr_node)
	{
		register_syntax_error("expression expected");
		return nullptr;
	}
	expect_tk(TokenType::T_SEMICOLON, "`;` expected after expression");

	expr_stmt_node
		->expressions
		.push_back(move(expr_node));

	return expr_stmt_node;
}

std::unique_ptr<ASTNode> Parser::expression_n(Precedence min_precedence)
{
	std::unique_ptr<ASTNode> left = operand_n();
	if (!left)
		return nullptr;
	consume_tk();

	while (!curr_tk->is_eof() && curr_tk->type() == TokenType::T_OPERATOR_SYM)
	{
		auto op = operator_n();
		if (!op || op->op_info->precedence < min_precedence)
			break;

		consume_tk(); // consume operator

		if (op->op_info->fixity == Fixity::F_LEFT) // left associative
		{
			auto right = expression_n((Precedence)((int)op->op_info->precedence + 1));
			left = std::make_unique<ExpressionNode>(
				ExpressionNode(
					move(left),
					move(op),
					move(right)
				)
			);
		}
		else // right associative
		{
			auto right = expression_n(op->op_info->precedence);
			left = std::make_unique<ExpressionNode>(
				ExpressionNode(
					move(left),
					move(op),
					move(right)
				)
			);
		}

		auto * expr = static_cast<ExpressionNode *>(left.get());
		left->line = expr->left->line;
		left->column = expr->left->column;
		left->start_position = expr->left->start_position;
		left->end_position = expr->right->end_position;
	}

	return left;
}

std::unique_ptr<ASTNode> Parser::operand_n(void)
{
	std::unique_ptr<OperandNode> operand_node { new OperandNode };
	operand_node->line = curr_tk->line();
	operand_node->column = curr_tk->column();
	operand_node->start_position = curr_tk->position();

	operand_node->op = operator_n(true);
	if (operand_node->op)
		consume_tk(); // consume operator
	operand_node->primary = primary_n();

	if (operand_node->primary)
		operand_node->end_position = operand_node->primary->end_position;
	else
		return nullptr;

	if (!operand_node->op)
	{
		auto & primary = operand_node->primary;
		if (primary)
			return move(primary);
	}
	
	return operand_node;
}

std::unique_ptr<ASTNode> Parser::primary_n(void)
{
	if (curr_tk->is_literal())
	{
		return literal_n();
	}
	else if (curr_tk->is_identifier() && next_tk->type() == TokenType::T_LEFT_PAREN)
	{
		return function_call_n();
	}
	else if (curr_tk->is_identifier())
	{
		return identifier_n();
	}
	else if (curr_tk->type() == TokenType::T_LEFT_PAREN)
	{
		size_t line = curr_tk->line();
		size_t column = curr_tk->column();
		size_t start_position = curr_tk->position();

		consume_tk(); // consume `(`
		auto expr = expression_n(P_MIN);
		if (!expr)
		{
			register_syntax_error("expression expected");
			return nullptr;
		}

		expr->line = line;
		expr->column = column;
		expr->start_position = start_position;
		expect_tk(TokenType::T_RIGHT_PAREN, "`)` expected");

		expr->end_position = curr_tk->position() + 1;

		return expr;
	}
	else if (curr_tk->type() == TokenType::T_OPERATOR_SYM)
	{
		return operand_n();
	}

	return nullptr;
}

std::unique_ptr<LiteralNode> Parser::literal_n(void)
{
	std::unique_ptr<LiteralNode> lit_node { new LiteralNode };

	switch (curr_tk->type())
	{
		case TokenType::T_INTEGER_LITERAL:
			lit_node->type = MathObjType(MOT::MO_INTEGER);
			break;
		case TokenType::T_REAL_LITERAL:
			lit_node->type = MathObjType(MOT::MO_REAL);
			break;
		
		default: return nullptr;
	}

	lit_node->value = curr_tk->lexeme();
	lit_node->line = curr_tk->line();
	lit_node->column = curr_tk->column();
	lit_node->start_position = curr_tk->position();
	lit_node->end_position = lit_node->start_position + curr_tk->lexeme().length();
	return lit_node;
}

std::unique_ptr<IdentifierNode> Parser::identifier_n(void)
{
	if (curr_tk->type() != TokenType::T_IDENTIFIER)
		return nullptr;
	std::unique_ptr<IdentifierNode> id_node {
		new IdentifierNode(curr_tk->lexeme())
	};

	id_node->line = curr_tk->line();
	id_node->column = curr_tk->column();
	id_node->start_position = curr_tk->position();
	id_node->end_position = id_node->start_position + curr_tk->lexeme().length();
	return id_node;
}

std::unique_ptr<OperatorNode> Parser::operator_n(bool unary)
{
	if (curr_tk->type() != TokenType::T_OPERATOR_SYM)
		return nullptr;
	if (unary && curr_tk->lexeme() == "+")
	{
		consume_tk();
		return nullptr;
	}

	auto op = operators->find(curr_tk->lexeme());
	if (!op)
	{
		if (unary)
			register_syntax_error("unary operator `" + std::string(curr_tk->lexeme()) + "` is not defined");
		else
			register_syntax_error("binary operator `" + std::string(curr_tk->lexeme()) + "` is not defined");
		return nullptr;
	}

	std::unique_ptr<OperatorNode> op_node {
		new OperatorNode(op)
	};
	op_node->line = curr_tk->line();
	op_node->column = curr_tk->column();
	op_node->start_position = curr_tk->position();
	op_node->end_position = op_node->start_position + curr_tk->lexeme().length();
	return op_node;
}

std::unique_ptr<TypeNode> Parser::type_n(void)
{
	std::unique_ptr<TypeNode> type_node { new TypeNode };

	if (curr_tk->type() == TokenType::T_CONST)
	{
		type_node->type.is_const = true;
		consume_tk();
	}
	else
	{
		type_node->type.is_const = false;
	}

	switch (curr_tk->type())
	{
		case TokenType::T_INTEGER:
			type_node->type.type = MOT::MO_INTEGER;
			break;
		case TokenType::T_REAL:
			type_node->type.type = MOT::MO_REAL;
			break;
		case TokenType::T_NONE:
			type_node->type.type = MOT::MO_NONE;
			break;

		default:
			return nullptr;
	}

	return type_node;
}

void Parser::expect_tk(TokenType type, std::string message)
{
	if (curr_tk->type() != type)
	{
		register_syntax_error(message);
		return;
	}
}

void Parser::register_syntax_error(std::string message)
{
	if (!panic_mode)
	{
		panic_mode = true;

		std::unique_ptr<Error> err { new SyntaxError(
			message,
			curr_tk->line(),
			curr_tk->column(),
			curr_tk->position(),
			curr_tk->lexeme().length()
		)};
		ErrorHandler::push_error(err);
	}
	synchronize();
}

void Parser::synchronize(void)
{
	while (curr_tk->type() != TokenType::T_EOF &&
		   curr_tk->type() != TokenType::T_SEMICOLON &&
		   curr_tk->type() != TokenType::T_LET &&
		   curr_tk->type() != TokenType::T_DEFINE &&
		   next_tk->type() != TokenType::T_ARROW)
	{
		consume_tk();
	}
}

bool Parser::consume_tk(void)
{
	if (curr_tk && next_tk)
	{
		curr_tk = std::move(next_tk);
		next_tk = lexer->scan_tk();
	}
	else
	{
		curr_tk = lexer->scan_tk();
		next_tk = lexer->scan_tk();
	}

	if (config::print_lexer_output)
		d_print_token(curr_tk);
	
	if (curr_tk->type() == TokenType::T_ERROR)
		panic_mode = true;

	return curr_tk->type() != TokenType::T_EOF;
}