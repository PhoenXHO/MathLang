#include "object/integer_object.hpp"
#include "object/real_object.hpp"


MathObjPtr IntegerObj::cast_to(const ClassPtr & cls)
{
	if (cls == Builtins::integer_class)
	{
		return shared_from_this();
	}
	if (cls == Builtins::real_class)
	{
		return std::make_shared<RealObj>(
			mpfr_float(m_value),
			m_size,
			0
		);
	}

	return nullptr;
}

MathObjPtr IntegerObj::add(const MathObjPtr & rhs) const
{
	if (rhs->is_instance_of(Builtins::integer_class))
	{
		auto rhs_int = std::dynamic_pointer_cast<IntegerObj>(rhs);
		return std::make_shared<IntegerObj>(m_value + rhs_int->value());
	}
	else if (rhs->is_instance_of(Builtins::real_class))
	{
		auto rhs_real = std::dynamic_pointer_cast<RealObj>(rhs);
		return std::make_shared<RealObj>(
			add_reals(m_value, rhs_real->value()),
			std::max(m_size, rhs_real->integer_part()),
			std::max(m_size, rhs_real->decimal_part())
		);
	}
	return MathObj::none;
}