#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "object/object.hpp"


struct Variable;
using VariablePtr = std::shared_ptr<Variable>;


struct Variable : public MathObj
{
	bool static_type = false;

	Variable(std::string_view name, MathObjPtr value) :
		MathObj(Builtins::reference_class),
		m_name(name),
		m_value(value)
	{}
	Variable(std::string_view name, ClassPtr cls);
	~Variable() = default;

	std::string to_string(void) const override;

	MathObjPtr add(const MathObjPtr & rhs) const override
	{ return m_value->add(rhs); }

	void set(MathObjPtr value)
	{ this->m_value = value; }

	const MathObjPtr & value(void) const
	{ return m_value ? m_value : MathObj::none; }

	ClassPtr value_class(void) const
	{ return m_value ? m_value->get_class() : Builtins::none_class; }

	std::string name(void) const
	{ return m_name; }

private:
	std::string m_name;
	MathObjPtr m_value;
};

#endif // VARIABLE_HPP