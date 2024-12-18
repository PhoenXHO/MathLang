#include "class/builtins.hpp"
#include "class/none_class.hpp"


void NoneClass::init(void)
{}

ObjectPtr NoneClass::instantiate(const std::any & value) const
{
	return Object::none;
}

ObjectPtr NoneClass::default_value(void) const
{
	return Object::none;
}

bool NoneClass::can_cast_to(const ClassPtr & cls) const
{
	return false;
}

ObjectPtr NoneClass::cast(const ObjectPtr & obj) const
{
	return Object::none;
}