#include <iostream>
#include <memory>

#include "vm.h"
#include "globals.h"
#include "error.h"
#include "semanalyzer.h"

bool config::print_lexer_output = false;
bool config::print_parser_output = false;
bool config::print_compiler_output = false;

void VM::interpret_source(std::string_view source)
{
	if (config::print_lexer_output)
		std::cout << ">>>>> Tokens <<<<<\n";
	Lexer lexer(source);

	Parser parser(lexer);
	parser.parse_source();

	if (ErrorHandler::has_errors())
	{
		ErrorHandler::report_errors(source);
		return;
	}

	if (config::print_parser_output)
	{
		std::cout << "\n>>>>> Parse Tree <<<<<\n";
		parser.get_ast().print();
	}

	SemanticAnalyzer semantic_analyzer(
		parser.get_ast(),
		std::move(parser.operators),
		current_scope
	);
	semantic_analyzer.analyze_source();

	if (ErrorHandler::has_errors())
	{
		ErrorHandler::report_errors(source);
		return;
	}

	Compiler compiler(
		parser.get_ast(),
		parser.operators,
		current_scope,
		variables, functions, operators
	);
	compiler.compile_source();
	chunk = compiler.chunk;
	chunk->ip = chunk->bytecode().begin();

	if (ErrorHandler::has_errors())
	{
		ErrorHandler::report_errors(source);
		return;
	}
	
	if (config::print_compiler_output)
	{
		std::cout << "\n>>>>> Bytecode <<<<<\n";
		compiler.disassemble();
	}

	run();
}

void VM::run(void)
{
	// Define helper macros for reading bytecode
	#define READ_BYTE()				(*(chunk->ip++))
	#define READ_CONSTANT()			(chunk->constants[READ_BYTE()])
	#define READ_VARIABLE()			((*variables)[READ_BYTE()])
	#define READ_FUNCTION()			((*functions)[READ_BYTE()])
	#define READ_OPERATOR()			((*operators)[READ_BYTE()].first)

	while (true)
	{
	uint8_t byte;
	switch (byte = READ_BYTE())
	{
		case OpCode::OP_LOAD_CONST:
		{
			// Load a constant value from the compiler's constants table
			auto & constant = READ_CONSTANT();
			stack.push(constant);
			break;
		}

		case OpCode::OP_ENTER_BLOCK:
			// Enter a new block scope (the first child scope of the current scope)
			enter_scope(current_scope, 0);
			break;
		case OpCode::OP_LEAVE_BLOCK:
			// Leave the current block scope
			pop_scope(current_scope);
			break;

		case OpCode::OP_CALL_FUNCTION:
		{
			// Call a function
			auto & function = READ_FUNCTION();
			if (function->type == FunctionType::F_BUILTIN)
			{
				throw std::runtime_error("builtin functions not implemented");
			}
			else if (function->type == FunctionType::F_CUSTOM)
			{
				auto custom_function = std::static_pointer_cast<CustomFunction>(function);

				// Enter function scope
				enter_function(current_scope, custom_function);

				// Set the current chunk to the function's chunk
				custom_function->chunk->parent = chunk;
				chunk = custom_function->chunk;
				chunk->ip = chunk->bytecode().begin();

				break;
			}

			// For debugging
			throw std::runtime_error("unknown function type");
		}
		// This will only be executed if the function has no return statement
		// All returning functions will have a return statement
		case OpCode::OP_LEAVE_FUNCTION:
			// Push None to the stack
			stack.push(std::shared_ptr<None>());
			// Leave function scope
			pop_function(current_scope);
			// Set the current chunk to the parent chunk
			chunk = chunk->parent;
			break;

		case OpCode::OP_SET_VAR:
		{
			// Set the value of a variable
			auto & variable = READ_VARIABLE();
			auto value = stack.top();
			stack.pop();
			variable->value = get_value(value);
			break;
		}
		case OpCode::OP_LOAD_VAR:
		{
			// Load the value of a variable
			auto & variable = READ_VARIABLE();
			stack.push(variable);
			break;
		}

		case OpCode::OP_UNARY_OP:
		{
			// Read the operator from the compiler's operators table
			auto & op = READ_OPERATOR();
			if (op->type == OperatorType::O_CUSTOM)
			{
				throw std::runtime_error("custom operators not implemented");
			}
			else if (op->type == OperatorType::O_BUILTIN)
			{
				auto operand = stack.top();
				stack.pop();

				auto _ = std::shared_ptr<MathObj>(new None());
				auto result = op->implementation(_, operand);
				stack.push(result);
				break;
			}
			
			// For debugging
			throw std::runtime_error("unknown operator type");
		}
		case OpCode::OP_BINARY_OP:
		{
			// Read the operator from the compiler's operators table
			auto & op = READ_OPERATOR();
			if (op->type == OperatorType::O_CUSTOM)
			{
				throw std::runtime_error("custom operators not implemented");
			}
			else
			{
				auto rhs = stack.top();
				stack.pop();
				auto lhs = stack.top();
				stack.pop();

				switch (op->type)
				{
					case OperatorType::O_BUILTIN:
					{
						auto result = op->implementation(lhs, rhs);
						stack.push(result);
						break;
					}
					case OperatorType::O_CUSTOM:
						throw std::runtime_error("custom operators not implemented");
						break;
				}
			}
			break;
		}

		case OpCode::OP_POP:
			stack.pop();
			break;
		case OpCode::OP_RETURN:
			if (chunk->parent)
			{
				// Push None to the stack
				stack.push(std::shared_ptr<None>());

				// Leave function scope
				pop_function(current_scope);

				// Set the current chunk to the parent chunk
				chunk = chunk->parent;
				break;
			}
			// If there is no parent chunk, then we are in the global scope and at the end of the program
			return;
		case OpCode::OP_RETURN_VALUE:
			// Leave function scope
			pop_function(current_scope);
			// Set the current chunk to the parent chunk
			chunk = chunk->parent;
			break;
	}
	}
}

std::shared_ptr<MathObj> get_value(std::shared_ptr<MathObj> & obj)
{
	if (obj->type().type == MOT::MO_VARIABLE)
	{
		auto variable = std::static_pointer_cast<Variable>(obj);
		return variable->value;
	}
	return obj;
}