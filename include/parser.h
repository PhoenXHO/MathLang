#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <string_view>

#include "ast.h"
#include "token.h"
#include "operator.h"
#include "lexer.h"

class Parser
{
private:
	std::unique_ptr<Lexer> lexer;
	AST ast;

	std::unique_ptr<Token> curr_tk;
	std::unique_ptr<Token> next_tk;

	bool panic_mode;

	bool consume_tk(void);
	void expect_tk(TokenType type, std::string message);
	void register_syntax_error(std::string message);
	void synchronize(void);

	std::unique_ptr<ASTNode>					statement_n(void)						;
	std::unique_ptr<VariableDeclarationNode>	variable_declaration_n(void)			;
	std::unique_ptr<ExpressionStatementNode>	expression_statement_n(void)			;
	std::unique_ptr<ASTNode>					expression_n(Precedence min_precedence)	;
	std::unique_ptr<ASTNode>					operand_n(void)							;
	std::unique_ptr<ASTNode>					primary_n(void)							;
	std::unique_ptr<LiteralNode>				literal_n(void)							;
	std::unique_ptr<IdentifierNode>				identifier_n(void)						;
	std::unique_ptr<OperatorNode>				operator_n(bool unary = false)			;
	std::unique_ptr<TypeNode>					type_n(void)							;
	
public:
	std::unique_ptr<OperatorTable> operators;

	Parser(Lexer & lexer);

	const AST & get_ast(void) const { return ast; }
	void parse_source(void);
};

#endif // PARSER_H