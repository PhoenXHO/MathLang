#include "class/class.hpp"

bool Class::can_cast_to(const ClassPtr & cls) const
{
	// Check if the class is the same
	if (shared_from_this() == cls)
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