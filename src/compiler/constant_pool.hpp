#ifndef CONSTANT_POOL_HPP
#define CONSTANT_POOL_HPP

#include <vector>

#include "object/object.hpp"

class ConstantPool
{
	std::vector<MathObjPtr> constants;

public:
	ConstantPool() = default;
	~ConstantPool() = default;

	void add_constant(MathObjPtr obj)
	{
		constants.push_back(obj);
	}

	MathObjPtr operator[](size_t index) const
	{
		return constants.at(index);
	}

	size_t size(void) const
	{
		return constants.size();
	}
};

#endif // CONSTANT_POOL_HPP