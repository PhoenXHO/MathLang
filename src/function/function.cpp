#include "function/function.hpp"


FunctionImplentationPtr FunctionImplentationRegistry::define(const FunctionImplentationPtr & implementation)
{
	implementation_indices[implementation->signature()] = implementations.size();
	implementations.push_back(implementation);
	return implementation;
}

std::pair<size_t, FunctionImplentationPtr> FunctionImplentationRegistry::find_most_specific(const FunctionSignature & signature) const
{
	FunctionImplentationPtr most_specific;
	size_t most_specific_index = -1;
	int most_specific_specificity = -1;

	for (size_t i = 0; i < implementations.size(); ++i)
	{
		const auto & impl = implementations[i];
		int specificity = measure_specificity(signature, impl->signature());
		if (specificity > most_specific_specificity)
		{
			most_specific = impl;
			most_specific_index = i;
			most_specific_specificity = specificity;
		}
	}

	return { most_specific_index, most_specific };

	//for (const auto & impl : implementations)
	//{
	//	int specificity = impl.first.measure_specificity(signature);
	//	if (specificity > most_specific_specificity)
	//	{
	//		most_specific = impl.second;
	//		most_specific_specificity = specificity;
	//	}
	//}

	//return most_specific;
}

int FunctionImplentationRegistry::measure_specificity(const FunctionSignature & signature, const FunctionSignature & other) const
{
	if (signature == other)
		return 0;

	if (signature.parameters.size() != other.parameters.size())
		return -1;

	int specificity = 0;
	for (size_t i = 0; i < signature.parameters.size(); ++i)
	{
		if (signature.parameters[i].second == other.parameters[i].second)
			++specificity;
	}

	if (signature.return_type == other.return_type)
		++specificity;

	return specificity;
}