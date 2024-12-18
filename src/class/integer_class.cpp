#include "class/builtins.hpp"
#include "class/integer_class.hpp"
#include "object/integer_object.hpp"


void IntegerClass::init(void)
{}

ObjectPtr IntegerClass::instantiate(const std::any & value) const
{
	if (value.has_value())
	{
		return std::make_shared<IntegerObj>(std::any_cast<std::string_view>(value));
	}
	return std::make_shared<IntegerObj>();
}

ObjectPtr IntegerClass::default_value(void) const
{
	return std::make_shared<IntegerObj>();
}

bool IntegerClass::can_cast_to(const ClassPtr & cls) const
{
	return cls == Builtins::integer_class ||
		   cls == Builtins::real_class ||
		   Builtins::integer_class->is_sub_class(cls);
}

ObjectPtr IntegerClass::cast(const ObjectPtr & obj) const
{
	return obj->cast_to(Builtins::integer_class);
}