#include "function/function_implementation.hpp"


int FunctionSignature::measure_specificity(const FunctionSignature & other) const
{
	if (parameters.size() != other.parameters.size())
		return 0;

	int specificity = 0;
	for (size_t i = 0; i < parameters.size(); ++i)
	{
		specificity += other.parameters[i].second->measure_specificity(parameters[i].second);
	}

	return specificity;
}

std::string FunctionSignature::to_string(bool parameters_only) const
{
	std::string str = "(";
	for (size_t i = 0; i < parameters.size(); ++i)
	{
		if (i > 0)
			str += ", ";
		str += parameters[i].second->to_string();
	}
	str += ")";
	if (!parameters_only)
	{
		str += " -> " + return_type->to_string();
	}
	return str;
}