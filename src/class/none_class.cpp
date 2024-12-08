#include "class/builtins.hpp"
#include "class/class.hpp"


namespace Builtins::NoneClass
{
	ClassPtr init(void)
	{
		none_class = std::make_shared<Class>("None");
		return none_class;
	}
}