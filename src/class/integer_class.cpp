#include "class/builtins.hpp"
#include "class/class.hpp"
#include "object/integer_object.hpp"


namespace Builtins::IntegerClass
{
	ClassPtr init(void)
	{
		integer_class = std::make_shared<Class>("Integer");

		// Define the fields
		auto value = std::make_shared<Variable>("value", std::make_shared<IntegerObj>());
		integer_class->add_field(value);

		integer_class->instantiate = [](const std::any & value) -> MathObjPtr
		{
			return std::make_shared<IntegerObj>(std::any_cast<std::string_view>(value));
		};
		integer_class->can_cast_to = [](ClassPtr cls) -> bool
		{
			return cls == integer_class ||
				   cls == Builtins::real_class ||
				   integer_class->is_sub_class(cls);
		};
		integer_class->cast = [](MathObjPtr obj) -> MathObjPtr
		{
			return obj->cast_to(integer_class);
		};

		return integer_class;
	}
}