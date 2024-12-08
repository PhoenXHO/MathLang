#include "class/class.hpp"

bool Class::is_sub_class(const ClassPtr & cls, bool strict) const
{
	// Check if the class is the same
	if (!strict && shared_from_this() == cls)
	{
		return true;
	}

	// Check if the class is a subclass
	//for (const auto & base : bases)
	//{
	//	if (base->can_cast_to(cls))
	//	{
	//		return true;
	//	}
	//}

	return false;
}

int Class::measure_specificity(const ClassPtr & cls) const
{
	// Check if the class is the same
	if (shared_from_this() == cls)
	{
		return 2;
	}

	// Check if this class is castable to the given class
	if (can_cast_to(cls))
	{
		return 1;
	}

	return 0;
}