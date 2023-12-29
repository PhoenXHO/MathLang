#include <iostream>
#include <memory>

#include "vm.h"
#include "globals.h"
#include "error.h"
#include "typechecker.h"

bool config::print_lexer_output = false;
bool config::print_parser_output = false;
bool config::print_compiler_output = false;

VM::VM(std::string_view source) : source(source)
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

	TypeChecker type_checker(
		parser.get_ast(),
		std::move(parser.operators)
	);
	type_checker.check_types();

	if (ErrorHandler::has_errors())
	{
		ErrorHandler::report_errors(source);
		return;
	}

	compiler = std::make_unique<Compiler>(
		parser.get_ast(),
		parser.operators
	);
	compiler->compile_source();

	if (ErrorHandler::has_errors())
	{
		ErrorHandler::report_errors(source);
		return;
	}
	
	if (config::print_compiler_output)
	{
		std::cout << "\n>>>>> Bytecode <<<<<\n";
		compiler->print_bytecode();
	}
}

void VM::interpret_source(void)
{
	if (!compiler)
		return;

	run();
}

void VM::run(void)
{
	auto it = compiler->bytecode().begin();

	#define READ_BYTE()		(*(it++))
	#define READ_CONSTANT()	(compiler->constants[READ_BYTE()])
	#define READ_OPERATOR()	(compiler->operators[READ_BYTE()].first)

	uint8_t byte;
	while (true)
	{
	switch (byte = READ_BYTE())
	{
		case OpCode::OP_LOAD_CONST:
			{
				auto constant = READ_CONSTANT();
				stack.push(constant);
			}
			break;

		case OpCode::OP_UNARY_OP:
			{
				auto * op = READ_OPERATOR();
				if (op->type == OperatorType::O_CUSTOM)
				{
					throw std::runtime_error("custom operators not implemented");
				}
				else
				{
					auto operand = stack.top();
					stack.pop();

					switch (op->type)
					{
						case OperatorType::O_BUILTIN:
						{
							auto result = op->implementation(*operand, NoneValue());
							stack.push(result);
							break;
						}
						case OperatorType::O_CUSTOM:
							throw std::runtime_error("custom operators not implemented");
							break;
					}
				}
			}
			break;

		case OpCode::OP_BINARY_OP:
			{
				auto * op = READ_OPERATOR();
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
							auto result = op->implementation(*lhs, *rhs);
							stack.push(result);
							break;
						}
						case OperatorType::O_CUSTOM:
							throw std::runtime_error("custom operators not implemented");
							break;
					}
				}
			}
			break;

		case OpCode::OP_POP:
			stack.pop();
			break;
		
		case OpCode::OP_RETURN:
			return;
	}
	}
}