#include <iostream>

#include "vm/vm.hpp"
#include "global/config.hpp" // for `config::verbose`
#include "global/globals.hpp" // for `globals::error_handler`
#include "memory/memory.hpp" // for `set_dynamic_precision`

InterpretResult VM::interpret_source(std::string_view source)
{
	this->source = source;

	set_dynamic_precision();

	try
	{
		compiler->compile_source(source);
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
	case OP_SET_VARIABLE_POP:
		{
			auto index = READ_BYTE();
			auto value = stack.top();
			stack.pop();
			// Set the variable in the current scope to the value on the stack
			current_scope->set_variable(index, value);
		}
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