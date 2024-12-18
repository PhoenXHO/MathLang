#include "class/builtins.hpp"
#include "class/mathobj_class.hpp"


void MathObjClass::init(void)
{}

ObjectPtr MathObjClass::instantiate(const std::any & value) const
{
	return Object::none;
}

ObjectPtr MathObjClass::default_value(void) const
{
	return Object::none;
}

bool MathObjClass::can_cast_to(const ClassPtr & cls) const
{
	return false;
}

ObjectPtr MathObjClass::cast(const ObjectPtr & obj) const
{
	return Object::none;
}