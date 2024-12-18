#pragma once

#include <vector>

#include "object/object.hpp"


class ConstantPool
{
	std::vector<ObjectPtr> constants;

public:
	ConstantPool() = default;
	~ConstantPool() = default;

	void add_constant(ObjectPtr obj)
	{
		constants.push_back(obj);
	}

	ObjectPtr operator[](size_t index) const
	{
		return constants.at(index);
	}

	size_t size(void) const
	{
		return constants.size();
	}
};