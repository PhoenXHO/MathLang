#include <iostream>

#include "object/object.hpp" // for `MathObjPtr`
#include "class/builtins.hpp"
#include "function/function_implementation.hpp"
#include "class/builtins.hpp"
#include "function/function.hpp"
#include "symbol/symbol_table.hpp"


// For testing purposes
namespace Builtins
{
	FunctionImplentationPtr print0 = std::make_shared<BuiltinFunctionImplentation>(
		FunctionSignature(),
		[](const std::vector<MathObjPtr> & arguments) -> MathObjPtr
		{
			std::cout << "None" << std::endl;
			return MathObj::none;
		}
	);
	FunctionImplentationPtr print1 = std::make_shared<BuiltinFunctionImplentation>(
		FunctionSignature({ { "object", Builtins::mathobj_class } }),
		[](const std::vector<MathObjPtr> & arguments) -> MathObjPtr
		{
			std::cout << arguments[0]->to_string() << std::endl;
			return MathObj::none;
		}
	);
	FunctionImplentationPtr print2 = std::make_shared<BuiltinFunctionImplentation>(
		FunctionSignature({ { "object1", Builtins::mathobj_class }, { "object2", Builtins::mathobj_class } }),
		[](const std::vector<MathObjPtr> & arguments) -> MathObjPtr
		{
			std::cout << arguments[0]->to_string() << ' ' << arguments[1]->to_string() << std::endl;
			return MathObj::none;
		}
	);
}


void SymbolTable::init_builtin_functions(void)
{
	auto print = define_function("print");
	print->define(Builtins::print0);
	print->define(Builtins::print1);
	print->define(Builtins::print2);
}