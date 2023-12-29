#include "parser.h"
#include "globals.h"
#include "ast.h"
#include "error.h"
#include <iostream>

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
		
		case TokenType::T_EOF:
			return nullptr;

		default:
			return expression_statement_n();
	}
}

std::unique_ptr<VariableDeclarationNode> Parser::variable_declaration_n(void)
{
	consume_tk(); // consume `let`
	std::unique_ptr<VariableDeclarationNode> var_dec_node { new VariableDeclarationNode };

	var_dec_node->type = type_n();
	if (!var_dec_node->type)
		return nullptr;
	consume_tk();
	
	var_dec_node->name = identifier_n();
	if (!var_dec_node->name)
	{
		register_syntax_error("identifier expected after type");
		return nullptr;
	}
	consume_tk();

	if (curr_tk->type() == TokenType::T_OPERATOR_SYM && curr_tk->lexeme() == ":=")
	{
		consume_tk(); // consume `:=`
		var_dec_node->expression = expression_n(P_MIN);
	}
	else
	{
		var_dec_node->expression = nullptr;
	}
	expect_tk(TokenType::T_SEMICOLON, "`;` expected after variable declaration");

	return var_dec_node;
}

std::unique_ptr<ExpressionStatementNode> Parser::expression_statement_n(void)
{
	std::unique_ptr<ExpressionStatementNode> expr_stmt_node { new ExpressionStatementNode };

	auto expr_node = expression_n(P_MIN);
	expect_tk(TokenType::T_SEMICOLON, "`;` expected after expression");

	expr_stmt_node
		->expressions
		.push_back(move(expr_node));

	return expr_stmt_node;
}

std::unique_ptr<ASTNode> Parser::expression_n(Precedence min_precedence)
{
	std::unique_ptr<ASTNode> left = operand_n();
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
		left->column = expr->left->column;
		left->line = expr->left->line;
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

	if (!operand_node->op)
	{
		auto & primary = operand_node->primary;
		if (primary && (primary->n_type == NodeType::N_EXPR || primary->n_type == NodeType::N_OPERAND))
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
	else
	{
		register_syntax_error("expression expected");
	}

	return nullptr;
}

std::unique_ptr<LiteralNode> Parser::literal_n(void)
{
	std::unique_ptr<LiteralNode> lit_node { new LiteralNode };

	switch (curr_tk->type())
	{
		case TokenType::T_INTEGER_LITERAL:
			lit_node->type = MathObjType::MO_INTEGER;
			break;
		case TokenType::T_REAL_LITERAL:
			lit_node->type = MathObjType::MO_REAL;
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

	auto * op = operators->find(curr_tk->lexeme());
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

	switch (curr_tk->type())
	{
		case TokenType::T_INTEGER:
			type_node->type = MathObjType::MO_INTEGER;
			break;
		case TokenType::T_REAL:
			type_node->type = MathObjType::MO_REAL;
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
		   curr_tk->type() != TokenType::T_LET)
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

	return curr_tk->type() != TokenType::T_EOF;
}