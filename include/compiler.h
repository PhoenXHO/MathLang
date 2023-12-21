#ifndef COMPILER_H
#define COMPILER_H

#include <memory>
#include <vector>
#include <unordered_map>

#include "ast.h"
#include "mathobj.h"
#include "operator.h"

class Compiler
{
	const AST & ast;
	std::unique_ptr<OperatorTable> operator_table;
	std::vector<uint8_t> bytes;

	void emit(uint8_t op_code);
	void emit(uint8_t op_code, uint8_t arg);

	void compile_statement			(const ASTNode * statement_n)					;
	void compile_expression			(const ASTNode * expression_n)					;
	void compile_operand			(const OperandNode * operand_n)					;
	void compile_operator			(const OperatorNode * operator_n, bool unary)	;
	void compile_binary_operator	(const OperatorNode * operator_n)				;
	void compile_unary_operator		(const OperatorNode * operator_n)				;
	void compile_identifier			(const IdentifierNode * identifier_n)			;
	void compile_literal			(const LiteralNode * literal_n)					;
	void compile_constant			(const LiteralNode * literal_n)					;

public:
	Compiler(const AST & ast, std::unique_ptr<OperatorTable> & operator_table) :
		ast(ast),
		operator_table(std::move(operator_table))
	{}

	enum OpCode
	{
		OP_LOAD_CONST,

		OP_UNARY_OP,
		OP_BINARY_OP,

		OP_POP,
		OP_RETURN
	};

	std::vector<std::shared_ptr<MathObj>> constants;
	std::vector<const Operator *> operators;

	void compile_source(void);

	void print_bytecode(void);
	void print_constant(std::shared_ptr<MathObj> & constant);
	void print_operator(const Operator * op);

	const std::vector<uint8_t> & bytecode(void) { return bytes; }
};
typedef Compiler::OpCode OpCode;

#endif // COMPILER_H