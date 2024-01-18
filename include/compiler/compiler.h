#ifndef COMPILER_H
#define COMPILER_H

#include <memory>
#include <vector>
#include <unordered_map>

#include "chunk.h"
#include "ast.h"
#include "mathobj.h"
#include "operator.h"
#include "scope.h"

class Compiler
{
	const AST & ast;
	std::unique_ptr<OperatorTable> operator_table;

	std::shared_ptr<std::vector<std::shared_ptr<Variable>>> variables;
	std::shared_ptr<std::vector<std::shared_ptr<Function>>> functions;
	std::shared_ptr<std::vector<std::pair<std::shared_ptr<const OperatorFunction>, std::string>>> operators;

	void emit(uint8_t op_code);
	void emit(uint8_t op_code, uint8_t arg);

	void compile_block					(const BlockNode * block_n)						;
	void compile_return_statement		(const ReturnStatementNode * return_statement_n);
	void compile_function_declaration	(const FunctionDeclarationNode * func_decl_n)	;
	void compile_function_call			(const FunctionCallNode * func_call_n)			;
	void compile_parameter				(const ParameterNode * parameter_n)				;
	void compile_statement				(const ASTNode * statement_n)					;
	void compile_variable_declaration	(const VariableDeclarationNode * var_decl_n)	;
	void compile_expression				(const ASTNode * expression_n)					;
	void compile_operand				(const OperandNode * operand_n)					;
	void compile_operator				(const OperatorNode * operator_n, bool unary)	;
	void compile_binary_operator		(const OperatorNode * operator_n)				;
	void compile_unary_operator			(const OperatorNode * operator_n)				;
	void compile_identifier				(const IdentifierNode * identifier_n)			;
	void compile_literal				(const LiteralNode * literal_n)					;
	void compile_constant				(const LiteralNode * literal_n)					;

	void register_compile_error(std::string message, std::string additional_info, const ASTNode * node);

public:
	enum OpCode
	{
		OP_LOAD_CONST,

		OP_ENTER_BLOCK,
		OP_LEAVE_BLOCK,

		OP_CALL_FUNCTION,
		OP_LEAVE_FUNCTION,

		OP_SET_VAR,
		OP_LOAD_VAR,

		OP_UNARY_OP,
		OP_BINARY_OP,

		OP_POP,
		OP_RETURN,
		OP_RETURN_VALUE,
	};
	
	Compiler(
		const AST & ast,
		std::unique_ptr<OperatorTable> & operator_table,
		std::shared_ptr<Scope> & scope,
		std::shared_ptr<std::vector<std::shared_ptr<Variable>>> variables,
		std::shared_ptr<std::vector<std::shared_ptr<Function>>> functions,
		std::shared_ptr<std::vector<std::pair<std::shared_ptr<const OperatorFunction>, std::string>>> operators
	) :
		ast(ast),
		operator_table(std::move(operator_table)),
		scope(scope),
		chunk(new Chunk("<main>")),
		variables(std::move(variables)),
		functions(std::move(functions)),
		operators(std::move(operators))
	{}

	std::shared_ptr<Scope> scope;
	std::shared_ptr<Chunk> chunk;

	void compile_source(void);

	void disassemble(void);
	void disassemble(std::shared_ptr<Chunk> & chunk);
	void print_constant(std::shared_ptr<MathObj> & constant);
	void print_variable(std::shared_ptr<Variable> & variable);
	void print_function(std::shared_ptr<Function> & function);
	void print_operator(std::pair<std::shared_ptr<const OperatorFunction>, std::string> & op);
};
typedef Compiler::OpCode OpCode;

#endif // COMPILER_H