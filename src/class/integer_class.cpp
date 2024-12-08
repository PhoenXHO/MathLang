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

		return integer_class;
	}
}