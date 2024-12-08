#include "class/builtins.hpp"
#include "class/class.hpp"


namespace Builtins::ReferenceClass
{
	ClassPtr init(void)
	{
		reference_class = std::make_shared<Class>("Reference");
		return reference_class;
	}
}