#pragma once

#include "object/object.hpp"


class RealObj : public Object
{
	mpfr_float m_value;
	size_t m_integer_part;
	size_t m_decimal_part;

public:
	RealObj(std::string_view value) :
		Object(Builtins::real_class),
		m_value(value)
	{
		size_t dot_pos = value.find('.');
		m_integer_part = dot_pos;
		m_decimal_part = value.size() - dot_pos - 1;
	}
	RealObj(const mpfr_float & value, size_t integer_part, size_t decimal_part) :
		Object(Builtins::real_class),
		m_value(value),
		m_integer_part(integer_part),
		m_decimal_part(decimal_part)
	{}
	RealObj() :
		Object(Builtins::real_class),
		m_value(0),
		m_integer_part(1),
		m_decimal_part(0)
	{}
	~RealObj() = default;

	mpfr_float value(void) const
	{ return m_value; }
	size_t integer_part(void) const
	{ return m_integer_part; }
	size_t decimal_part(void) const
	{ return m_decimal_part; }

	ObjectPtr cast_to(const ClassPtr & cls) override;

	std::string to_string(void) const override
	{
		std::ostringstream oss;
		oss << std::setprecision(m_integer_part + m_decimal_part) << m_value;
		return oss.str();

		//char * str = new char[integer_part + decimal_part + 2];
		//mpfr_sprintf(str, "%.*Rf", integer_part + decimal_part, value.backend().data());
		//std::string result(str);
		//delete[] str;
		//return result;
	}

	ObjectPtr add(const ObjectPtr & rhs) const override;
};