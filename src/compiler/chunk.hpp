#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <string>
#include <vector>
#include <iostream>

#include "compiler/constant_pool.hpp"

enum OpCode : uint8_t
{
	// Instruction to load a constant from the constant pool into the stack
	// - operand: `index` (1 byte); the index of the constant in the constant pool (`Compiler.constant_pool`)
	OP_LOAD_CONSTANT,

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

	Chunk(std::string name) : name(name), constant_pool() {}
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
	MathObjPtr read_constant(void)
	{
		uint8_t index = read_byte();
		return constant_pool[index];
	}

	void clear_code(void)
	{
		code.clear();
		// We don't want to clear the constant pool since it will be used for the entire program
		// The garbage collector will take care of removing unused constants
	}

	//* Debugging
	void disassemble(void) const;

private:
	std::string name;
	std::vector<uint8_t> code;
	std::vector<uint8_t>::const_iterator ip;
	ConstantPool constant_pool; //TODO: Optimize storage of constants to storing duplicates
};

#endif // CHUNK_HPP