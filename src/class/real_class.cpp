#include "class/builtins.hpp"
#include "class/real_class.hpp"
#include "object/real_object.hpp"


void RealClass::init(void)
{
	// Define the fields
	auto value = std::make_shared<Variable>("value", std::make_shared<RealObj>());
	add_field(value);

	instantiate = [](const std::any & value) -> MathObjPtr
	{
		return std::make_shared<RealObj>(std::any_cast<std::string_view>(value));
	};
	default_value = []() -> MathObjPtr
	{
		return std::make_shared<RealObj>();
	};
	can_cast_to = [](ClassPtr cls) -> bool
	{
		return cls == Builtins::real_class ||
			   Builtins::real_class->is_sub_class(cls);
	};
	cast = [](MathObjPtr obj) -> MathObjPtr
	{
		return obj->cast_to(Builtins::real_class);
	};
}