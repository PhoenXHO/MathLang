#include "function.h"

std::pair<FuncImplementations::const_iterator, FuncImplementations::const_iterator> FunctionTable::get_implementations(std::string_view name) const
{
	std::string s_name(name);
	return functions.equal_range(s_name);
}

void FunctionTable::register_function(std::string name, std::shared_ptr<Function> func)
{
	functions.insert({ name, func });
}