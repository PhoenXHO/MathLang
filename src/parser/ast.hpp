#pragma once

#include <memory>
#include <vector>
#include <string_view>
#include <iostream>

#include "operator/operator.hpp"
#include "object/object.hpp"
#include "util/util.hpp"
#include "class/class.hpp"


struct AST;
struct ASTNode;
struct VariableDeclarationNode;
struct ExpressionStatementNode;
struct ExpressionNode;
struct OperandNode;
struct OperatorNode;
struct FunctionCallNode;
struct IdentifierNode;
struct TypeNode;
struct LiteralNode;


// <program> ::= <statement>*
// <statement> ::= <expression-statement> (for now)
struct AST
{
	std::vector<std::unique_ptr<ASTNode>> statements;

	AST() = default;
	~AST() = default;
	
	//* Debugging
	friend std::ostream & operator<<(std::ostream & os, const AST & ast);
};

struct ASTNode
{
	enum class Type
	{
		N_STATEMENT,
		N_VARIABLE_DECLARATION,
		N_EXPRESSION_STATEMENT,
		N_EXPRESSION,
		N_OPERAND,
		N_OPERATOR,
		N_FUNCTION_CALL,
		N_IDENTIFIER,
		N_TYPE,
		N_LITERAL
	};

	const Type type;
	SourceLocation location;
	size_t length;

	ASTNode(Type type) : type(type) {}
	virtual ~ASTNode() = default;

	//* Debugging
	virtual void print(int depth = 0) const = 0;
};

// <variable-declaration> ::= "let" [ <type> ] <identifier> [ ":=" <expression> ] [ ";" ]
struct VariableDeclarationNode : public ASTNode
{
	std::unique_ptr<TypeNode> type;
	std::unique_ptr<IdentifierNode> identifier;
	std::unique_ptr<ASTNode> expression;
	bool print_expression = false;

	VariableDeclarationNode() : ASTNode(Type::N_VARIABLE_DECLARATION) {}
	~VariableDeclarationNode() = default;

	//* Debugging
	void print(int depth = 0) const override;
};

// <expression-statement> ::= ( <expression> | <operand> ) [ ";" ]
struct ExpressionStatementNode : public ASTNode
{
	std::unique_ptr<ASTNode> expression;
	bool print_expression = false;

	ExpressionStatementNode() : ASTNode(Type::N_EXPRESSION_STATEMENT) {}
	ExpressionStatementNode(std::unique_ptr<ASTNode> expression)
		: ASTNode(Type::N_EXPRESSION_STATEMENT), expression(std::move(expression))
	{}
	~ExpressionStatementNode() = default;

	//* Debugging
	void print(int depth = 0) const override;
};

// <expression> ::= ( <operand> | <expression> ) <operator> ( <operand> | <expression> )
struct ExpressionNode : public ASTNode
{
	std::unique_ptr<ASTNode> left;
	std::unique_ptr<ASTNode> right;
	std::unique_ptr<OperatorNode> op;

	ExpressionNode() : ASTNode(Type::N_EXPRESSION) {}
	ExpressionNode(
		std::unique_ptr<ASTNode> left,
		std::unique_ptr<OperatorNode> op,
		std::unique_ptr<ASTNode> right
	) : ASTNode(Type::N_EXPRESSION), left(std::move(left)), op(std::move(op)), right(std::move(right))
		{}
	~ExpressionNode() = default;

	//* Debugging
	void print(int depth = 0) const override;
};

// <operand> ::= <operator> <primary>
// <primary> ::= <literal> | <operand> | <expression>
struct OperandNode : public ASTNode
{
	std::unique_ptr<OperatorNode> op;
	std::unique_ptr<ASTNode> primary;

	OperandNode() : ASTNode(Type::N_OPERAND) {}
	~OperandNode() = default;

	//* Debugging
	void print(int depth = 0) const override;
};

// <operator> ::= <custom-operator> | <built-in-operator>
struct OperatorNode : public ASTNode
{
	std::shared_ptr<Operator> op;
	std::shared_ptr<OperatorImplentation> implementation; // To store the implementation of the operator for the given operands (for the compiler)
	size_t operator_index = 0;

	OperatorNode(std::shared_ptr<Operator> op) : ASTNode(Type::N_OPERATOR), op(op) {}
	~OperatorNode() = default;

	Fixity fixity(void) const
	{ return op->fixity(); }
	Associativity associativity(void) const
	{ return op->associativity(); }
	Precedence precedence(void) const
	{ return op->precedence(); }
	OperatorImplentationRegistry & implementations(void)
	{ return op->implementations(); }

	//* Debugging
	void print(int depth = 0) const override;
};

// <function-call> ::= <identifier> "(" [ <expression> [ "," <expression> ]* ] ")"
struct FunctionCallNode : public ASTNode
{
	std::unique_ptr<IdentifierNode> identifier;
	std::vector<std::unique_ptr<ASTNode>> arguments;
	size_t function_index = 0;
	size_t function_implementation_index = 0;

	FunctionCallNode() : ASTNode(Type::N_FUNCTION_CALL) {}
	~FunctionCallNode() = default;

	//* Debugging
	void print(int depth = 0) const override;
};

// <identifier> ::= <identifier>
struct IdentifierNode : public ASTNode
{
	std::string_view name;
	size_t variable_index = 0;

	IdentifierNode() : ASTNode(Type::N_IDENTIFIER) {}
	IdentifierNode(std::string_view name) : ASTNode(Type::N_IDENTIFIER), name(name) {}
	~IdentifierNode() = default;

	//* Debugging
	void print(int depth = 0) const override;
};

// <type> ::= <identifier>
struct TypeNode : public ASTNode
{
	std::string_view name;

	TypeNode() : ASTNode(Type::N_TYPE) {}
	TypeNode(std::string_view name) : ASTNode(Type::N_TYPE), name(name) {}
	~TypeNode() = default;

	//* Debugging
	void print(int depth = 0) const override;
};

// <literal> ::= <integer> | <real> (for now)
struct LiteralNode : public ASTNode
{
	ClassPtr cls;
	std::string_view value;

	LiteralNode() : ASTNode(Type::N_LITERAL) {}
	LiteralNode(std::string_view value) : ASTNode(Type::N_LITERAL), value(value) {}
	~LiteralNode() = default;

	//* Debugging
	void print(int depth = 0) const override;
};