#include "object/object.hpp"

mpfr add_reals(const mpfr & lhs, const mpfr & rhs);

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
		return std::make_shared<RealObj>(add_reals(value, rhs_real->get_value()));
	}
	else
	{
		throw std::runtime_error("Invalid type for addition");
	}
}

MathObjPtr RealObj::add(const MathObjPtr & rhs) const
{
	if (rhs->type() == Type::MO_INTEGER || rhs->type() == Type::MO_REAL)
	{
		auto rhs_real = std::dynamic_pointer_cast<RealObj>(rhs);
		return std::make_shared<RealObj>(add_reals(value, rhs_real->get_value()));
	}
	else
	{
		throw std::runtime_error("Invalid type for addition");
	}
}

mpfr add_reals(const mpfr & lhs, const mpfr & rhs)
{
	mpfr result = lhs + rhs;
	result.precision(std::max(lhs.precision(), rhs.precision()));
	return result;
}