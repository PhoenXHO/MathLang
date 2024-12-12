#include "class/builtins.hpp"
#include "class/none_class.hpp"


void NoneClass::init(void)
{
	instantiate = [](const std::any & value) -> MathObjPtr
	{
		return MathObj::none;
	};
	default_value = []() -> MathObjPtr
	{
		return MathObj::none;
	};
	can_cast_to = [](ClassPtr cls) -> bool
	{
		return false; // The none object cannot be cast to any class
	};
	cast = [](MathObjPtr obj) -> MathObjPtr
	{
		return nullptr; // The none object cannot be cast to any class
	};
}