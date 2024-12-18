#include "class/builtins.hpp"
#include "class/reference_class.hpp"


void ReferenceClass::init(void)
{}

ObjectPtr ReferenceClass::instantiate(const std::any & value) const
{
	return Object::none;
}

ObjectPtr ReferenceClass::default_value(void) const
{
	return Object::none;
}

bool ReferenceClass::can_cast_to(const ClassPtr & cls) const
{
	return false;
}

ObjectPtr ReferenceClass::cast(const ObjectPtr & obj) const
{
	return Object::none;
}