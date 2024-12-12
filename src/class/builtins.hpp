#pragma once

#include <memory>


class Class;
using ClassPtr = std::shared_ptr<Class>;


namespace Builtins
{
	extern ClassPtr mathobj_class; // The base class for all objects
	extern ClassPtr reference_class; // The class for references (for variables)
	extern ClassPtr none_class; // The class for the none object

	extern ClassPtr integer_class; // The class for integer objects
	extern ClassPtr real_class; // The class for real objects

	extern void init_builtin_classes(void);
}