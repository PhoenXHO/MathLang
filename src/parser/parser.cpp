#include <iostream>

#include "parser/parser.hpp"
#include "global/globals.hpp"

void Parser::parse_source(std::string_view source)
{
	lexer->set_source(source);

	// Keep scanning new tokens until we reach the end of the file
	while (consume_tk())
	{
		// Parse a statement and add it to the AST
		ast
			.statements
			.push_back(statement_n());
		// We have reached the end of the statement, so we can turn off panic mode
		panic_mode = false;
	}

	// Check for any syntax errors and report them
	globals::error_handler.check_errors();

	//* Debugging
	std::cout << ast;
}

std::unique_ptr<ASTNode> Parser::statement_n(void)
{
	switch (curr_tk->type())
	{
	case Token::Type::T_EOF:
		return nullptr;
	
	default:
		// For now, we only have expression statements
		return expression_statement_n();
	}
}

std::unique_ptr<ASTNode> Parser::expression_statement_n(void)
{
	auto expression = expression_n(Precedence::P_MIN);
	if (!expression)
	{
		//globals::error_handler.log_syntax_error("Expected an expression", curr_tk->line(), curr_tk->column(), curr_tk->position(), true);

		// Instead of logging an error directly, we can make use of the `expect_tk` function which will log the error for us
		// and, if the code is incomplete, set the `incomplete_code` flag to `true`
		expect_tk(Token::Type::T_NONE, "Expected an expression"); // We don't need to check for a specific token type, so we can use `T_NONE`
	}
	// The semicolon is optional, but if it is present, we need to consume it
	//expect_tk(Token::Type::T_SEMICOLON, "Expected a semicolon at the end of the statement");

	if (curr_tk->type() == Token::Type::T_SEMICOLON)
	{
		consume_tk(); // Consume the semicolon token
	}

	return expression;
}

std::unique_ptr<ASTNode> Parser::expression_n(Precedence min_p)
{
	auto left = operand_n();
	if (!left)
	{
		return nullptr;
	}
	consume_tk();

	while (!curr_tk->is_eof() && curr_tk->is_operator())
	{
		auto op = operator_n();
		if (!op || op->get_precedence() < min_p)
		{
			break;
		}

		consume_tk(); // Consume the operator token

		if (op->get_associativity() == Associativity::A_LEFT) // Left-associative
		{
			auto right = expression_n(static_cast<Precedence>(op->get_precedence() + 1));
			if (!right)
			{
				//globals::error_handler.log_syntax_error("Expected an expression", curr_tk->line(), curr_tk->column(), curr_tk->position(), true);

				expect_tk(Token::Type::T_NONE, "Expected an expression");
			}

			left = std::make_unique<ExpressionNode>(std::move(left), std::move(op), std::move(right));
		}
		else if (op->get_associativity() == Associativity::A_RIGHT) // Right-associative
		{
			auto right = expression_n(op->get_precedence());
			if (!right)
			{
				//globals::error_handler.log_syntax_error("Expected an expression", curr_tk->line(), curr_tk->column(), curr_tk->position(), true);

				expect_tk(Token::Type::T_NONE, "Expected an expression");
			}

			left = std::make_unique<ExpressionNode>(std::move(left), std::move(op), std::move(right));
		}
		else // Non-associative
		{
			globals::error_handler.log_syntax_error("Non-associative operators are not supported", curr_tk->line(), curr_tk->column(), curr_tk->position(), true);
		}
	}

	return left;
}

std::unique_ptr<ASTNode> Parser::operand_n(void)
{
	auto operand = std::make_unique<OperandNode>();

	operand->op = operator_n(true); // Unary operator
	if (operand->op)
	{
		consume_tk(); // Consume the operator token
	}
	
	operand->primary = primary_n();
	if (!operand->primary)
	{
		return nullptr;
	}

	//TODO: Handle postfix operators

	// If the operand is a primary, return it
	// This is to reduce the number of nodes in the AST and simplify the evaluation process
	if (!operand->op)
	{
		auto & primary = operand->primary;
		if (primary)
		{
			return std::move(primary);
		}
	}

	return operand;
}

std::unique_ptr<OperatorNode> Parser::operator_n(bool is_unary)
{
	if (!curr_tk->is_operator())
	{
		return nullptr;
	}
	if (is_unary && curr_tk->lexeme() == "+")
	{
		consume_tk(); // Skip the unary plus (no effect)
		return nullptr;
	}

	auto op = operator_table.find(curr_tk->lexeme());
	if (!op)
	{
		if (is_unary)
		{
			globals::error_handler.log_syntax_error(
				"Invalid unary operator '" + std::string(curr_tk->lexeme()) + "'",
				curr_tk->line(),
				curr_tk->column(),
				curr_tk->position(),
				true
			);
		}
		else
		{
			globals::error_handler.log_syntax_error(
				"Invalid binary operator '" + std::string(curr_tk->lexeme()) + "'",
				curr_tk->line(),
				curr_tk->column(),
				curr_tk->position(),
				true
			);
		}
	}

	auto operator_node = std::make_unique<OperatorNode>(op);
	return operator_node;
}

std::unique_ptr<ASTNode> Parser::primary_n(void)
{
	if (curr_tk->is_literal())
	{
		return literal_n();
	}
	else if (curr_tk->type() == Token::Type::T_LEFT_PAREN)
	{
		consume_tk(); // Consume the left parenthesis
		auto expression = expression_n(Precedence::P_MIN);
		if (!expression)
		{
			//globals::error_handler.log_syntax_error("Expected an expression", curr_tk->line(), curr_tk->column(), curr_tk->position(), true);

			expect_tk(Token::Type::T_NONE, "Expected an expression");
		}
		expect_tk(Token::Type::T_RIGHT_PAREN, "Expected a right parenthesis");
		// We don't need to consume the right parenthesis token because it will be consumed in the `expression_n` function

		return expression;
	}

	return nullptr;
}

std::unique_ptr<LiteralNode> Parser::literal_n(void)
{
	auto literal = std::make_unique<LiteralNode>();

	// For now, we can only parse numbers
	switch (curr_tk->type())
	{
	case Token::Type::T_INTEGER_LITERAL:
		literal->type = MathObj::Type::MO_INTEGER;
		break;

	case Token::Type::T_REAL_LITERAL:
		literal->type = MathObj::Type::MO_REAL;
		break;

	default:
		return nullptr;
	}

	literal->value = curr_tk->lexeme();
	return literal;
}

void Parser::expect_tk(Token::Type type, std::string_view message)
{
	if (curr_tk->type() != type)
	{
		if (next_tk->type() == Token::Type::T_EOF)
		{
			globals::error_handler.incomplete_code = true;
		}

		globals::error_handler.log_syntax_error(
			message.empty() ? "Expected token of type " + Token::type_to_string(type) : message,
			curr_tk->line(),
			curr_tk->column(),
			curr_tk->position(),
			true
		);
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

	// If the current token is an error token, enter panic mode
	if (curr_tk->type() == Token::Type::T_ERROR)
	{
		panic_mode = true;

		//TODO: Handle error token
	}

	return curr_tk->type() != Token::Type::T_EOF;
}