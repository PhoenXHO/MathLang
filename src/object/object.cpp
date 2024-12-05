#include "object/object.hpp"

mpfr_float add_reals(const mpfr_float & lhs, const mpfr_float & rhs);

MathObjPtr IntegerObj::add(const MathObjPtr & rhs) const
{
	if (rhs->type() == Type::MO_INTEGER)
	{
		auto rhs_int = std::dynamic_pointer_cast<IntegerObj>(rhs);
		return std::make_shared<IntegerObj>(m_value + rhs_int->value());
	}
	else if (rhs->type() == Type::MO_REAL)
	{
		auto rhs_real = std::dynamic_pointer_cast<RealObj>(rhs);
		return std::make_shared<RealObj>(
			add_reals(m_value, rhs_real->value()),
			std::max(m_size, rhs_real->integer_part()),
			std::max(m_size, rhs_real->decimal_part())
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
		mpfr_float real_value(m_value);
		mpfr_float int_value(rhs_int->value()); // Convert the integer to a real with the sufficient precision
		real_value = add_reals(real_value, int_value);
		return std::make_shared<RealObj>(
			real_value,
			std::max(m_integer_part, rhs_int->size()),
			m_decimal_part
		);
	}
	else if (rhs->type() == Type::MO_REAL)
	{
		auto rhs_real = std::dynamic_pointer_cast<RealObj>(rhs);
		auto result = add_reals(m_value, rhs_real->value());
		return std::make_shared<RealObj>(
			result,
			std::max(m_integer_part, rhs_real->integer_part()),
			std::max(m_decimal_part, rhs_real->decimal_part())
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