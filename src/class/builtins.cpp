#include "class/builtins.hpp"
#include "class/mathobj_class.hpp"
#include "class/reference_class.hpp"
#include "class/none_class.hpp"
#include "class/integer_class.hpp"
#include "class/real_class.hpp"


namespace Builtins
{
	ClassPtr mathobj_class;
	ClassPtr reference_class;
	ClassPtr none_class;

	ClassPtr integer_class;
	ClassPtr real_class;

	void init_builtin_classes(void)
	{
		mathobj_class = std::make_shared<MathObjClass>();
		reference_class = std::make_shared<ReferenceClass>();
		none_class = std::make_shared<NoneClass>();

		integer_class = std::make_shared<IntegerClass>();
		real_class = std::make_shared<RealClass>();
	}
}