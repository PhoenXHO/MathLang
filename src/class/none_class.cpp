#include "class/builtins.hpp"
#include "class/class.hpp"


namespace Builtins::NoneClass
{
	ClassPtr init(void)
	{
		none_class = std::make_shared<Class>("None");

		none_class->instantiate = nullptr;
		none_class->can_cast_to = [](ClassPtr cls) -> bool
		{
			return false; // The none object cannot be cast to any class
		};
		none_class->cast = [](MathObjPtr obj) -> MathObjPtr
		{
			return nullptr; // The none object cannot be cast to any class
		};
		return none_class;
	}
}