#include "class/builtins.hpp"
#include "class/integer_class.hpp"
#include "object/integer_object.hpp"


void IntegerClass::init(void)
{
	// Define the fields
	auto value = std::make_shared<Variable>("value", std::make_shared<IntegerObj>());
	add_field(value);

	instantiate = [](const std::any & value) -> MathObjPtr
	{
		return std::make_shared<IntegerObj>(std::any_cast<std::string_view>(value));
	};
	default_value = []() -> MathObjPtr
	{
		return std::make_shared<IntegerObj>();
	};
	can_cast_to = [](ClassPtr cls) -> bool
	{
		return cls == Builtins::integer_class ||
			   cls == Builtins::real_class ||
			   Builtins::integer_class->is_sub_class(cls);
	};
	cast = [](MathObjPtr obj) -> MathObjPtr
	{
		return obj->cast_to(Builtins::integer_class);
	};
}