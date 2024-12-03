#include "object/object.hpp"

mpfr_float add_reals(const mpfr_float & lhs, const mpfr_float & rhs);

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
		return std::make_shared<RealObj>(
			add_reals(value, rhs_real->get_value()),
			std::max(size, rhs_real->get_integer_part()),
			std::max(size, rhs_real->get_decimal_part())
		);
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
		mpfr_float real_value(value);
		mpfr_float int_value(rhs_int->get_value()); // Convert the integer to a real with the sufficient precision
		real_value = add_reals(real_value, int_value);
		return std::make_shared<RealObj>(
			real_value,
			std::max(integer_part, rhs_int->get_size()),
			decimal_part
		);
	}
	else if (rhs->type() == Type::MO_REAL)
	{
		auto rhs_real = std::dynamic_pointer_cast<RealObj>(rhs);
		auto result = add_reals(value, rhs_real->get_value());
		return std::make_shared<RealObj>(
			result,
			std::max(integer_part, rhs_real->get_integer_part()),
			std::max(decimal_part, rhs_real->get_decimal_part())
		);
	}
	else
	{
		throw std::runtime_error("Invalid type for addition");
	}
}

mpfr_float add_reals(const mpfr_float & lhs, const mpfr_float & rhs)
{
	auto lhs_precision = mpfr_get_prec(lhs.backend().data());
	auto rhs_precision = mpfr_get_prec(rhs.backend().data());
	auto max_precision = std::max(lhs_precision, rhs_precision);

	mpfr_t result;
	mpfr_init2(result, max_precision);

	mpfr_add(result, lhs.backend().data(), rhs.backend().data(), MPFR_RNDN);
	mpfr_float result_mpfr(result); // Convert the result to an mpfr object
	mpfr_clear(result); // Clear the temporary result

	return result_mpfr;
}