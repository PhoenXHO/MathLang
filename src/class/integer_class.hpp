#pragma once

#include "class.hpp"

struct IntegerClass : public Class
{
	IntegerClass(void) : Class("Integer") { init(); }

	void init(void);
};