#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <string>
#include <vector>
#include <iostream>

#include "compiler/constant_pool.hpp"
#include "scope/scope.hpp"

enum OpCode : uint8_t
{
	// Instruction to load a constant from the constant pool into the stack
	// - operand: `index` (1 byte); the index of the constant in the constant pool (`Compiler.constant_pool`)
	OP_LOAD_CONSTANT,

	// Instruction to set a variable in the current scope without popping the top of the stack
	// - operand: `index` (1 byte); the index of the variable in the current scope
	OP_SET_VARIABLE,
	// Instruction to set a variable in the current scope
	// - operand: `index` (1 byte); the index of the variable in the current scope
	OP_SET_VARIABLE_POP,
	// Instruction to get a variable from the current scope
	// - operand: `index` (1 byte); the index of the variable in the current scope
	OP_GET_VARIABLE,

	// Instruction to call a unary operator
	// - operand: `index` (1 byte); the index of the operator in the operator stack (`Compiler.operator_stack`)
	OP_CALL_UNARY,
	// Instruction to call a binary operator
	// - operand: `index` (1 byte); the index of the operator in the operator stack (`Compiler.operator_stack`)
	OP_CALL_BINARY,

	// Instruction to print the top of the stack (the top of the stack is popped)
	// - no operand
	OP_PRINT,

	// Instruction to pop the top of the stack
	// - no operand
	OP_POP,
	// Instruction to signal the end of a chunk
	// - no operand
	OP_RETURN
};

struct Chunk
{
	// Grant access to the Compiler class
	friend class Compiler;

	Chunk(std::string name, ConstantPool & constant_pool, std::shared_ptr<Scope> global_scope) :
		name(name), constant_pool(constant_pool), global_scope(global_scope)
	{}
	~Chunk() = default;

	void emit(OpCode op)
	{
		code.push_back(static_cast<uint8_t>(op));
	}
	void emit(OpCode op, uint8_t operand)
	{
		code.push_back(static_cast<uint8_t>(op));
		code.push_back(operand);
	}
	void emit_at(uint8_t index, OpCode op)
	{
		if (index >= code.size())
		{
			code.push_back(static_cast<uint8_t>(op));
		}
		else
		{
			code[index] = static_cast<uint8_t>(op);
		}
	}
	void emit_constant(uint8_t index)
	{
		emit(OP_LOAD_CONSTANT);
		code.push_back(index);
	}

	void init_ip(void)
	{ ip = code.cbegin(); }
	uint8_t read_byte(void)
	{ return *ip++; }

	void clear_code(void)
	{
		code.clear();
	}

	//* Debugging
	void disassemble(void) const;

private:
	std::string name;
	std::vector<uint8_t> code;
	std::vector<uint8_t>::const_iterator ip;
	ConstantPool & constant_pool;
	std::shared_ptr<Scope> global_scope;
	std::shared_ptr<Scope> current_scope = global_scope;

	//* Debugging
	std::vector<uint8_t>::const_iterator disassemble_instruction(std::vector<uint8_t>::const_iterator ip) const;
};

#endif // CHUNK_HPP