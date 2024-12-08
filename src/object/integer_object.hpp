#pragma once

#include "object/object.hpp"


class IntegerObj : public MathObj
{
	mpz_int m_value;
	size_t m_size;

public:
	IntegerObj(std::string_view value) :
		MathObj(Builtins::integer_class),
		m_value(value),
		m_size(value.size())
	{}
	IntegerObj(const mpz_int & value) :
		MathObj(Builtins::integer_class),
		m_value(value),
		m_size(value.str().size())
	{}
	IntegerObj() :
		MathObj(Builtins::integer_class),
		m_value(0),
		m_size(1)
	{}
	~IntegerObj() = default;

	mpz_int value(void) const { return m_value; }
	size_t size(void) const { return m_size; }

	Type type(void) const override { return Type::MO_INTEGER; }
	std::string to_string(void) const override { return m_value.str(); }

	MathObjPtr add(const MathObjPtr & rhs) const override;
};