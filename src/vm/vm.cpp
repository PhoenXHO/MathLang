#include <iostream>

#include "vm/vm.hpp"
#include "util/config.hpp" // for `config::verbose`
#include "util/globals.hpp" // for `globals::error_handler`
#include "memory/memory.hpp" // for `set_dynamic_precision`
#include "util/benchmark.hpp" // for `Benchmark`

InterpretResult VM::interpret_source(bool interrupted)
{
	set_dynamic_precision();

	InterpretResult result = InterpretResult::OK;
	Benchmark benchmark;
	try
	{
		benchmark.start_benchmark();
		compiler->compile_source();
		run();
		benchmark.end_benchmark();

		// If there are any warnings, report them and return
		if (globals::error_handler.has_warnings())
		{
			globals::error_handler.report_errors();
			globals::error_handler.reset();
			result = InterpretResult::WARNING;
		}
	}
	catch (const ErrorArray & errors)
	{
		bool incomplete_code = globals::error_handler.is_incomplete_code();
		
		// If the code is incomplete, we don't want to log the errors (in REPL mode only)
		if (config::repl_mode && (interrupted || !incomplete_code))
		{
			errors.report_errors();
		}
		globals::error_handler.reset();
		
		compiler->reset();
		return incomplete_code ? InterpretResult::INCOMPLETE_CODE : globals::error_handler.get_most_severe_error();
	}

	benchmark.print_benchmark_result();
	compiler->reset();
	return result;
}

void VM::run(void)
{
	#define READ_BYTE() chunk.read_byte()
	#define READ_CONSTANT() constant_pool[READ_BYTE()]

	chunk.init_ip();
	while (true)
	{
	uint8_t instruction = READ_BYTE();
	switch (static_cast<OpCode>(instruction))
	{
	case OP_LOAD_CONSTANT:
		{
			auto constant = READ_CONSTANT();
			stack.push(constant);
		}
		break;

	case OP_SET_VARIABLE:
		{
			auto index = READ_BYTE();
			auto value = stack.top();
			// Set the variable in the current scope to the value on the stack
			current_scope->set_variable(index, value);
		}
		break;
	case OP_GET_VARIABLE:
		{
			auto index = READ_BYTE();
			auto value = current_scope->get_variable(index)->get_value();
			stack.push(value);
		}
		break;

	case OP_CALL_UNARY:
		{
			auto index = READ_BYTE();
			auto & implementation = compiler->get_operator(index);
			auto operand = stack.top();
			stack.pop();
			
			std::shared_ptr<BuiltinOperatorImplentation> impl = std::dynamic_pointer_cast<BuiltinOperatorImplentation>(implementation);
			auto result = impl->execute(operand, nullptr);
			stack.push(result);
		}
		break;
	case OP_CALL_BINARY:
		{
			auto index = READ_BYTE();
			auto & implementation = compiler->get_operator(index);
			auto right = stack.top();
			stack.pop();
			auto left = stack.top();
			stack.pop();
			
			std::shared_ptr<BuiltinOperatorImplentation> impl = std::dynamic_pointer_cast<BuiltinOperatorImplentation>(implementation);
			auto result = impl->execute(left, right);
			stack.push(result);
		}
		break;

	case OP_PRINT:
		{
			auto result = stack.top();
			stack.pop();
			std::cout << result->to_string() << '\n';
		}
		break;

	case OP_POP:
		stack.pop();
		break;
	case OP_RETURN:
		return;
	}
	}
}