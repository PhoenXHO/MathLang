#pragma once

#include "class.hpp"

struct MathObjClass : public Class
{
	MathObjClass(void) : Class("MathObj") { init(); }

	void init(void);
};