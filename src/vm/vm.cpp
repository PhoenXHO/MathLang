#include <iostream>

#include "vm/vm.hpp"
#include "global/config.hpp" // for `config::verbose`
#include "global/globals.hpp" // for `globals::error_handler`

InterpretResult VM::interpret_source(std::string_view source)
{
	this->source = source;

	if (!compiler)
	{
		compiler = std::make_unique<Compiler>();
	}

	try
	{
		compiler->compile_source(source);
		compiler->init_main_ip();
		run();
	}
	catch (const ErrorArray & errors)
	{
		bool incomplete_code = globals::error_handler.is_incomplete_code();
		compiler->reset();

		// If the code is incomplete, we don't want to log the errors
		if (!incomplete_code)
		{
			errors.report_errors();
		}
		globals::error_handler.reset();
		
		return incomplete_code ? InterpretResult::INCOMPLETE_CODE : globals::error_handler.get_most_severe_error();
	}

	compiler->reset();
	return InterpretResult::OK;
}

void VM::run(void)
{
	auto & chunk = compiler->get_chunk();
	while (true)
	{
	uint8_t instruction = chunk.read_byte();
	switch (static_cast<OpCode>(instruction))
	{
	case OP_LOAD_CONSTANT:
		{
			auto constant = chunk.read_constant();
			stack.push(constant);
		}
		break;

	case OP_CALL_UNARY:
		{
			auto index = chunk.read_byte();
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
			auto index = chunk.read_byte();
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

	case OP_RETURN:
		// The result is at the top of the stack
		{
			auto result = stack.top();
			stack.pop();
			std::cout << result->to_string() << '\n';
			return;
		}
		return;
	}
	}
}