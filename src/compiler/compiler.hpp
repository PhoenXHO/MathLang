#ifndef COMPILER_H
#define COMPILER_H

#include <string_view>
#include <memory>
#include <vector>

#include "parser/parser.hpp"
#include "semantic_analyzer/semantic_analyzer.hpp"
#include "compiler/chunk.hpp"
#include "parser/ast.hpp"

class Compiler
{
	std::unique_ptr<Parser> parser;
	std::unique_ptr<SemanticAnalyzer> semantic_analyzer;
	Chunk & chunk; // Reference to the chunk in the VM
	std::vector<std::shared_ptr<OperatorImplentation>> operator_stack;

	void compile_statement      (const ASTNode *)      				          ;
	void compile_expression     (const ASTNode *)                             ;
	void compile_operand        (const OperandNode *)                         ;
	void compile_operator       (const OperatorNode *, bool is_unary = false) ;
	void compile_unary_operator (const OperatorNode *) 					      ;
	void compile_binary_operator(const OperatorNode *) 					      ;
	void compile_literal        (const LiteralNode *)  					      ;

public:
	Compiler(Chunk & chunk) :
		chunk(chunk),
		parser(std::make_unique<Parser>()),
		semantic_analyzer(std::make_unique<SemanticAnalyzer>())
	{}
	~Compiler() = default;

	void reset(void)
	{
		parser->reset();
		chunk.clear_code();
	}

	void compile_source(std::string_view source);

	const std::shared_ptr<OperatorImplentation> & get_operator(size_t index) const
	{ return operator_stack.at(index); }
};

#endif // COMPILER_H