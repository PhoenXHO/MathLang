#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "object/object.hpp"


struct Variable;
using VariablePtr = std::shared_ptr<Variable>;


struct Variable : public Object
{
	bool static_type = false;

	Variable(std::string_view name, ObjectPtr value) :
		Object(Builtins::reference_class),
		m_name(name),
		m_value(value)
	{}
	Variable(std::string_view name, ClassPtr cls);
	~Variable() = default;

	std::string to_string(void) const override;

	ObjectPtr add(const ObjectPtr & rhs) const override
	{ return m_value->add(rhs); }

	void set(ObjectPtr value)
	{ this->m_value = value; }

	const ObjectPtr & value(void) const
	{ return m_value ? m_value : Object::none; }

	ClassPtr value_class(void) const
	{ return m_value ? m_value->get_class() : Builtins::none_class; }

	std::string name(void) const
	{ return m_name; }

	ObjectPtr cast_to(const ClassPtr & cls) override
	{ return m_value->cast_to(cls); }

private:
	std::string m_name;
	ObjectPtr m_value;
};

#endif // VARIABLE_HPP