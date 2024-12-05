#include <iostream>

#include "parser/parser.hpp"
#include "util/globals.hpp"

void Parser::parse_source(void)
{
	lexer->update_source();

	// Keep scanning new tokens until we reach the end of the file
	while (consume_tk())
	{
		auto statement = statement_n();
		if (!statement)
		{
			continue;
		}
		// Parse a statement and add it to the AST
		ast
			.statements
			.push_back(std::move(statement));
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
	case Token::Type::T_LET:
		return variable_declaration_n();

	case Token::Type::T_EOF: case Token::Type::T_SEMICOLON:
		return nullptr;
	
	default:
		// For now, we only have expression statements
		return expression_statement_n();
	}
}

std::unique_ptr<VariableDeclarationNode> Parser::variable_declaration_n(void)
{
	auto variable_declaration = std::make_unique<VariableDeclarationNode>();
	variable_declaration->location = curr_tk->location();

	consume_tk(); // Consume the `let` token

	if (curr_tk->type() != Token::Type::T_IDENTIFIER)
	{
		expect_tk(Token::Type::T_IDENTIFIER, "Expected an identifier");
	}

	variable_declaration->identifier = identifier_n();
	consume_tk(); // Consume the identifier token

	if (curr_tk->type() == Token::Type::T_COLON_EQUAL)
	{
		consume_tk(); // Consume the ':=' token

		// We can use `expression_statement_n` to parse the expression so as to avoid code duplication
		auto expression_statement = expression_statement_n();
		variable_declaration->expression = std::move(expression_statement->expression);
		variable_declaration->print_expression = expression_statement->print_expression;
	}
	
	variable_declaration->length = curr_tk->location().position - variable_declaration->location.position + 1;
	return variable_declaration;
}

std::unique_ptr<ExpressionStatementNode> Parser::expression_statement_n(void)
{
	auto expression = expression_n(Precedence::P_MIN);
	if (!expression)
	{
		// Instead of logging an error directly, we can make use of the `expect_tk` function which will log the error for us
		// and, if the code is incomplete, set the `incomplete_code` flag to `true`
		expect_tk(Token::Type::T_NONE, "Expected an expression"); // We don't need to check for a specific token type, so we can use `T_NONE`
	}

	auto statement = std::make_unique<ExpressionStatementNode>(std::move(expression));

	// The semicolon is optional, but if it is present, we need to consume it
	//expect_tk(Token::Type::T_SEMICOLON, "Expected a semicolon at the end of the statement");

	// If there is no semicolon, we print the expression
	bool semicolon = check_semicolon();
	if (!semicolon)
	{
		if (!at_newline)
		{
			// Location of the newline character
			SourceLocation location {
				prev_tk->location().line,
				prev_tk->location().column + prev_tk->lexeme().size(),
				prev_tk->location().position + prev_tk->lexeme().size()
			};
			globals::error_handler.log_warning(
				"Statments separated by whitespace only",
				location,
				1,
				"Consider using ';' or newlines to separate statements"
			);
		}

		// If there is no semicolon, retreat the token so that the next statement can parse it
		retreat_tk();
	}
	statement->print_expression = !semicolon;
	return statement;
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
				expect_tk(Token::Type::T_NONE, "Expected an expression");
			}

			left = std::make_unique<ExpressionNode>(std::move(left), std::move(op), std::move(right));
		}
		else if (op->get_associativity() == Associativity::A_RIGHT) // Right-associative
		{
			auto right = expression_n(op->get_precedence());
			if (!right)
			{
				expect_tk(Token::Type::T_NONE, "Expected an expression");
			}

			left = std::make_unique<ExpressionNode>(std::move(left), std::move(op), std::move(right));
		}
		else // Non-associative
		{
			globals::error_handler.log_syntax_error(
				"Non-associative operators are not supported",
				curr_tk->location(),
				1,
				true
			);
		}

		left->location = left->location;
		left->length = curr_tk->location().position - left->location.position + 1;
	}

	return left;
}

std::unique_ptr<ASTNode> Parser::operand_n(void)
{
	auto operand = std::make_unique<OperandNode>();
	operand->location = curr_tk->location();

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

	operand->length = curr_tk->location().position - operand->location.position + 1;
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
			//globals::error_handler.log_syntax_error(
			//	"Invalid unary operator '" + std::string(curr_tk->lexeme()) + "'",
			//	curr_tk->location(),
			//	curr_tk->lexeme().size(),
			//	true
			//);
			expect_tk(Token::Type::T_NONE, "Invalid unary operator '" + std::string(curr_tk->lexeme()) + "'");
		}
		else
		{
			//globals::error_handler.log_syntax_error(
			//	"Invalid binary operator '" + std::string(curr_tk->lexeme()) + "'",
			//	curr_tk->location(),
			//	curr_tk->lexeme().size(),
			//	true
			//);
			expect_tk(Token::Type::T_NONE, "Invalid binary operator '" + std::string(curr_tk->lexeme()) + "'");
		}
	}

	auto operator_node = std::make_unique<OperatorNode>(op);
	operator_node->location = curr_tk->location();
	operator_node->length = curr_tk->lexeme().size();
	return operator_node;
}

std::unique_ptr<ASTNode> Parser::primary_n(void)
{
	if (curr_tk->is_literal())
	{
		return literal_n();
	}
	else if (curr_tk->type() == Token::Type::T_IDENTIFIER)
	{
		return identifier_n();
	}
	else if (curr_tk->type() == Token::Type::T_LEFT_PAREN)
	{
		SourceLocation location = curr_tk->location();

		consume_tk(); // Consume the left parenthesis
		auto expression = expression_n(Precedence::P_MIN);
		if (!expression)
		{
			expect_tk(Token::Type::T_NONE, "Expected an expression");
		}
		expect_tk(Token::Type::T_RIGHT_PAREN, "Expected a right parenthesis");
		// We don't need to consume the right parenthesis token because it will be consumed in the `expression_n` function

		expression->location = location;
		expression->length = curr_tk->location().position - location.position + 1;
		return expression;
	}

	return nullptr;
}

std::unique_ptr<IdentifierNode> Parser::identifier_n(void)
{
	auto identifier = std::make_unique<IdentifierNode>(curr_tk->lexeme());
	identifier->location = curr_tk->location();
	identifier->length = curr_tk->lexeme().size();
	return identifier;
}

std::unique_ptr<LiteralNode> Parser::literal_n(void)
{
	auto literal = std::make_unique<LiteralNode>(curr_tk->lexeme());

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

	literal->location = curr_tk->location();
	literal->length = curr_tk->lexeme().size();	
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
			message,
			curr_tk->location(),
			curr_tk->lexeme().size(),
			true
		);
	}
}

void Parser::expect_tk(const std::initializer_list<Token::Type> & types, std::string_view message)
{
	for (auto type : types)
	{
		if (curr_tk->type() == type)
		{
			return;
		}
	}

	if (next_tk->type() == Token::Type::T_EOF)
	{
		globals::error_handler.incomplete_code = true;
	}

	globals::error_handler.log_syntax_error(
		message,
		curr_tk->location(),
		true
	);
}

bool Parser::consume_tk(void)
{
	at_newline = false;
	prev_tk = curr_tk;
	do
	{
		if (curr_tk && next_tk)
		{
			curr_tk = next_tk;
			next_tk = lexer->scan_tk();
		}
		else
		{
			curr_tk = lexer->scan_tk();
			next_tk = lexer->scan_tk();
		}

		if (curr_tk->type() == Token::Type::T_EOL || curr_tk->type() == Token::Type::T_EOF)
		{
			at_newline = true;
		}
	}
	while (curr_tk->type() == Token::Type::T_EOL);

	// If the current token is an error token, enter panic mode
	if (curr_tk->type() == Token::Type::T_ERROR)
	{
		panic_mode = true;

		//TODO: Handle error token
	}

	return curr_tk->type() != Token::Type::T_EOF;
}

void Parser::retreat_tk(void)
{
	if (prev_tk)
	{
		lexer->retreat_tk(next_tk);
		next_tk = curr_tk;
		curr_tk = prev_tk;
		prev_tk = nullptr;
	}
	else
	{
		//! This should never happen
		throw std::runtime_error("Cannot retreat to a token that does not exist");
	}
}