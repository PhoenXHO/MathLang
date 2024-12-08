#include "object/real_object.hpp"
#include "object/integer_object.hpp"


MathObjPtr RealObj::cast_to(const ClassPtr & cls)
{
	if (cls == Builtins::real_class)
	{
		return shared_from_this();
	}

	return nullptr;
}

MathObjPtr RealObj::add(const MathObjPtr & rhs) const
{
	if (rhs->is_instance_of(Builtins::integer_class))
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
	else if (rhs->is_instance_of(Builtins::real_class))
	{
		auto rhs_real = std::dynamic_pointer_cast<RealObj>(rhs);
		auto result = add_reals(m_value, rhs_real->value());
		return std::make_shared<RealObj>(
			result,
			std::max(m_integer_part, rhs_real->integer_part()),
			std::max(m_decimal_part, rhs_real->decimal_part())
		);
	}

	return MathObj::none;
}