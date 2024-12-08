#include "class/builtins.hpp"
#include "class/class.hpp"
#include "object/real_object.hpp"


namespace Builtins::RealClass
{
	ClassPtr init(void)
	{
		real_class = std::make_shared<Class>("Real");

		// Define the fields
		auto value = std::make_shared<Variable>("value", std::make_shared<RealObj>());
		real_class->add_field(value);

		real_class->instantiate = [](const std::any & value) -> MathObjPtr
		{
			return std::make_shared<RealObj>(std::any_cast<std::string_view>(value));
		};
		real_class->can_cast_to = [](ClassPtr cls) -> bool
		{
			return cls == real_class ||
				   real_class->is_sub_class(cls);
		};
		real_class->cast = [](MathObjPtr obj) -> MathObjPtr
		{
			return obj->cast_to(real_class);
		};

		return real_class;
	}
}