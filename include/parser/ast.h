#ifndef AST_H
#define AST_H

#include <memory>
#include <string_view>
#include <variant>
#include <vector>
#include <unordered_map>

#include "operator.h"
#include "function.h"

struct Operator;
struct BuiltinOperator;

struct AST;
struct ASTNode;
struct BlockNode;
struct FunctionDeclarationNode;
struct ParameterNode;
struct VariableDeclarationNode;
struct ExpressionStatementNode;
struct ExpressionNode;
struct OperandNode;
struct LiteralNode;
struct IdentifierNode;
struct OperatorNode;
struct TypeNode;

enum class NodeType
{
	N_STATEMENT,
	N_BLOCK,
	N_RETURN,
	N_RETURN_STMT,
	N_FUNC_DECL,
	N_PARAMETER,
	N_FUNC_CALL,
	N_EXPR_STMT,
	N_VAR_DECL,
	N_EXPR,
	N_OPERAND,
	N_LITERAL,
	N_IDENTIFIER,
	N_OPERATOR,
	N_TYPE
};

// <statement> ::= <expression-statement> | <block> | <variable-declaration>
struct AST
{
	std::vector<std::unique_ptr<ASTNode>> statements;

	void print(void) const;
};

struct ASTNode
{
	size_t line, column;
	size_t start_position, end_position;

	const NodeType n_type;
	ASTNode(NodeType n_type) : n_type(n_type) {}
	virtual ~ASTNode() = default;
	virtual void print(int depth) const = 0;
};

// <block> ::= "{" { <statement> } "}"
struct BlockNode : public ASTNode
{
	std::vector<std::unique_ptr<ASTNode>> statements;
	uint8_t relative_index;

	BlockNode(void) : ASTNode(NodeType::N_BLOCK) {}
	virtual void print(int depth) const override;
};

// <return> ::= "return"
struct ReturnNode : public ASTNode
{
	ReturnNode(void) : ASTNode(NodeType::N_RETURN) {}
	virtual void print(int depth) const override;
};

// <return-statement> ::= ":->" <expression>
struct ReturnStatementNode : public ASTNode
{
	std::unique_ptr<ASTNode> value;

	ReturnStatementNode(void) : ASTNode(NodeType::N_RETURN_STMT) {}
	virtual void print(int depth) const override;
};

// <function-declaration> ::= "define" <identifier> "(" [ <parameter> { "," <parameter> } ] ")" [ "->" <type> ] <block>
struct FunctionDeclarationNode : public ASTNode
{
	std::unique_ptr<IdentifierNode> name;
	std::vector<std::unique_ptr<ParameterNode>> parameters;
	std::unique_ptr<TypeNode> return_type;
	std::unique_ptr<BlockNode> body;
	std::shared_ptr<CustomFunction> function;

	FunctionDeclarationNode(void) : ASTNode(NodeType::N_FUNC_DECL) {}
	virtual void print(int depth) const override;
};

// <function-call> ::= <identifier> "(" [ <expression> { "," <expression> } ] ")"
struct FunctionCallNode : public ASTNode
{
	std::unique_ptr<IdentifierNode> name;
	std::vector<std::unique_ptr<ASTNode>> arguments;
	std::shared_ptr<Function> function;

	FunctionCallNode(void) : ASTNode(NodeType::N_FUNC_CALL) {}
	virtual void print(int depth) const override;
};

// <parameter> ::= <identifier> ":" <type> [ "=" <expression> ]
struct ParameterNode : public ASTNode
{
	std::unique_ptr<IdentifierNode> name;
	std::unique_ptr<TypeNode> type;
	std::unique_ptr<ASTNode> default_value;

	ParameterNode(void) : ASTNode(NodeType::N_PARAMETER) {}
	virtual void print(int depth) const override;
};

// <variable-declaration> ::= "let" <type> <identifier> [ ":=" ( <expression> | <node> ) ]
struct VariableDeclarationNode : public ASTNode
{
	std::unique_ptr<TypeNode> type;
	std::unique_ptr<IdentifierNode> name;
	std::unique_ptr<ASTNode> value;

	VariableDeclarationNode(void) : ASTNode(NodeType::N_VAR_DECL) {}
	virtual void print(int depth) const override;
};

// <expression-statement> ::= { <operand> | <expression> }
struct ExpressionStatementNode : public ASTNode
{
	std::vector<std::unique_ptr<ASTNode>> expressions;

	ExpressionStatementNode(void) : ASTNode(NodeType::N_EXPR_STMT) {}
	virtual void print(int depth) const override;
};

// <expression> ::= ( <operand> | <expression> ) <operator> ( <operand> | <expression> )
struct ExpressionNode : public ASTNode
{
	std::unique_ptr<ASTNode> left;
	std::unique_ptr<OperatorNode> op;
	std::unique_ptr<ASTNode> right;

	ExpressionNode(void) : ASTNode(NodeType::N_EXPR) {}
	ExpressionNode(
		std::unique_ptr<ASTNode> left,
		std::unique_ptr<OperatorNode> op,
		std::unique_ptr<ASTNode> right
	) : left(move(left)), op(move(op)), right(move(right)), ASTNode(NodeType::N_EXPR) {}

	virtual void print(int depth) const override;
};

// <operand> ::= <operator> <primary>
// <primary> ::= <literal> | <identifier> | ( <operand> | <expression> )
struct OperandNode : public ASTNode
{
	std::unique_ptr<OperatorNode> op;
	std::unique_ptr<ASTNode> primary;

	OperandNode(void) : ASTNode(NodeType::N_OPERAND) {}
	virtual void print(int depth) const override;
};

struct LiteralNode : public ASTNode
{
	MathObjType type;
	std::string_view value;

	LiteralNode(void) : ASTNode(NodeType::N_LITERAL) {}
	virtual void print(int depth) const override;
};

struct IdentifierNode : public ASTNode
{
	std::string_view name;

	IdentifierNode(std::string_view name) : name(name), ASTNode(NodeType::N_IDENTIFIER) {}
	virtual void print(int depth) const override;
};

struct OperatorNode : public ASTNode
{
	std::shared_ptr<const Operator> op_info;
	std::shared_ptr<const OperatorFunction> op_func;

	OperatorNode(std::shared_ptr<const Operator> op_info) :
		op_info(op_info),
		ASTNode(NodeType::N_OPERATOR)
	{}
	virtual void print(int depth) const override;
};

struct TypeNode : public ASTNode
{
	MathObjType type;

	TypeNode(void) : ASTNode(NodeType::N_TYPE) {}
	TypeNode(MathObjType type) : type(type), ASTNode(NodeType::N_TYPE) {}
	virtual void print(int depth) const override;
};

#endif // AST_H