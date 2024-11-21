#ifndef PARSER_HPP
#define PARSER_HPP

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

	std::unique_ptr<Token> curr_tk = nullptr;
	std::unique_ptr<Token> next_tk = nullptr;

	bool panic_mode = false;

	/// @brief Consume the current token and get the next token.
	/// @return `true` if the next token is not EOF, false otherwise.
	bool consume_tk(void);
	/// @brief Expect a token of a certain type.
	/// @param type: The type of token to expect.
	/// @param message: An optional message to display if the token is not of the expected type.
	void expect_tk(Token::Type type, std::string_view message = "");

	std::unique_ptr<ASTNode>      statement_n(void)                 ;
	std::unique_ptr<ASTNode>      expression_statement_n(void)      ;
	std::unique_ptr<ASTNode>      expression_n(Precedence min_p)    ;
	std::unique_ptr<ASTNode>      operand_n(void)                   ;
	std::unique_ptr<OperatorNode> operator_n(bool is_unary = false) ;
	std::unique_ptr<ASTNode>      primary_n(void)                   ;
	std::unique_ptr<LiteralNode>  literal_n(void)                   ;

public:
	OperatorTable operator_table;

	Parser()
	{
		operator_table.register_builtin_operators();
	}
	~Parser() = default;

	void reset(void)
	{
		lexer->reset();
		ast.statements.clear();
		curr_tk = nullptr;
		next_tk = nullptr;
		panic_mode = false;
	}

	void parse_source(std::string_view source);

	const AST & get_ast(void) const
	{ return ast; }
};

#endif // PARSER_HPP