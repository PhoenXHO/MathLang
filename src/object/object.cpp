#include "object/object.hpp"

MathObjPtr IntegerObj::add(const MathObjPtr & rhs) const
{
	if (rhs->type() == Type::MO_INTEGER)
	{
		auto rhs_int = std::dynamic_pointer_cast<IntegerObj>(rhs);
		return std::make_shared<IntegerObj>(value + rhs_int->get_value());
	}
	else if (rhs->type() == Type::MO_REAL)
	{
		auto rhs_real = std::dynamic_pointer_cast<RealObj>(rhs);
		return std::make_shared<RealObj>(value + rhs_real->get_value());
	}
	else
	{
		throw std::runtime_error("Invalid type for addition");
	}
}

MathObjPtr RealObj::add(const MathObjPtr & rhs) const
{
	if (rhs->type() == Type::MO_INTEGER)
	{
		auto rhs_int = std::dynamic_pointer_cast<IntegerObj>(rhs);
		return std::make_shared<RealObj>(value + rhs_int->get_value());
	}
	else if (rhs->type() == Type::MO_REAL)
	{
		auto rhs_real = std::dynamic_pointer_cast<RealObj>(rhs);
		return std::make_shared<RealObj>(value + rhs_real->get_value());
	}
	else
	{
		throw std::runtime_error("Invalid type for addition");
	}
}