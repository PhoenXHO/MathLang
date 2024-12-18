#include "class/builtins.hpp"
#include "class/real_class.hpp"
#include "object/real_object.hpp"


void RealClass::init(void)
{}

ObjectPtr RealClass::instantiate(const std::any & value) const
{
	if (value.has_value())
	{
		return std::make_shared<RealObj>(std::any_cast<std::string_view>(value));
	}
	return std::make_shared<RealObj>();
}

ObjectPtr RealClass::default_value(void) const
{
	return std::make_shared<RealObj>();
}

bool RealClass::can_cast_to(const ClassPtr & cls) const
{
	return cls == Builtins::real_class ||
		   Builtins::real_class->is_sub_class(cls);
}

ObjectPtr RealClass::cast(const ObjectPtr & obj) const
{
	return obj->cast_to(Builtins::real_class);
}