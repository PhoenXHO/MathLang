#pragma once

#include <string_view>
#include <memory>

#include "lexer/lexer.hpp"
#include "parser/ast.hpp"
#include "lexer/token.hpp"
#include "operator/operator.hpp"


class Parser
{
	// Keep a reference to the lexer to get tokens
	std::unique_ptr<Lexer> lexer;

	AST ast;

	std::shared_ptr<Token> prev_tk = nullptr;
	std::shared_ptr<Token> curr_tk = nullptr;
	std::shared_ptr<Token> next_tk = nullptr;

	bool panic_mode = false;
	// Keep track of whether the current token is preceded by a newline character (or end of file)
	bool at_newline = false;

	/// @brief Consume the current token and get the next token.
	/// @return `true` if the next token is not EOF, false otherwise.
	bool consume_tk(void);
	/// @brief Go back to the previous token.
	void retreat_tk(void);
	/// @brief Expect a token of a certain type.
	/// @param type: The type of token to expect.
	/// @param message: An optional message to display if the token is not of the expected type.
	void expect_tk(Token::Type type, std::string_view message = "");
	/// @brief Expect a token of a certain type from a list of types.
	/// @param types: A list of token types to expect.
	/// @param message: An optional message to display if the token is not of the expected type.
	void expect_tk(const std::initializer_list<Token::Type> & types, std::string_view message = "");
	/// @brief Check if the current token is a semicolon.
	/// @return `true` if the current token is a semicolon, `false` otherwise.
	bool check_semicolon(void)
	{
		if (curr_tk->type() == Token::Type::T_SEMICOLON)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	std::unique_ptr<ASTNode>                 statement_n(void)                 ;
	std::unique_ptr<VariableDeclarationNode> variable_declaration_n(void)      ;
	std::unique_ptr<ExpressionStatementNode> expression_statement_n(void)      ;
	std::unique_ptr<ASTNode>                 expression_n(Precedence min_p)    ;
	std::unique_ptr<ASTNode>                 operand_n(void)                   ;
	std::unique_ptr<OperatorNode>            operator_n(bool is_unary = false) ;
	std::unique_ptr<ASTNode>                 primary_n(void)                   ;
	std::unique_ptr<IdentifierNode>          identifier_n(void)                ;
	std::unique_ptr<LiteralNode>             literal_n(void)                   ;

public:
	OperatorRegistry & operators;

	Parser(OperatorRegistry & operators) :
		lexer(std::make_unique<Lexer>()),
		operators(operators)
	{}
	~Parser() = default;

	void reset(void)
	{
		lexer->reset();
		ast.statements.clear();
		prev_tk = nullptr;
		curr_tk = nullptr;
		next_tk = nullptr;
		panic_mode = false;
		at_newline = false;
	}

	void parse_source(void);

	const AST & get_ast(void) const
	{ return ast; }
};