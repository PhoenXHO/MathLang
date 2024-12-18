#pragma once

#include "object/object.hpp"


class IntegerObj : public Object
{
	mpz_int m_value;
	size_t m_size;

public:
	IntegerObj(std::string_view value) :
		Object(Builtins::integer_class),
		m_value(value),
		m_size(value.size())
	{}
	IntegerObj(const mpz_int & value) :
		Object(Builtins::integer_class),
		m_value(value),
		m_size(value.str().size())
	{}
	IntegerObj() :
		Object(Builtins::integer_class),
		m_value(0),
		m_size(1)
	{}
	~IntegerObj() = default;

	mpz_int value(void) const
	{ return m_value; }
	size_t size(void) const
	{ return m_size; }

	ObjectPtr cast_to(const ClassPtr & cls) override;

	std::string to_string(void) const override
	{ return m_value.str(); }
	ObjectPtr add(const ObjectPtr & rhs) const override;
};