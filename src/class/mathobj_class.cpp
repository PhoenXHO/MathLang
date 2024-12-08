#include "class/builtins.hpp"
#include "class/class.hpp"


namespace Builtins::MathObjClass
{
	ClassPtr init(void)
	{
		mathobj_class = std::make_shared<Class>("MathObj");
		return mathobj_class;
	}
}