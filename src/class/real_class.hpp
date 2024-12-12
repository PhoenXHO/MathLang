#pragma once

#include "class.hpp"


struct RealClass : public Class
{
	RealClass(void) : Class("Real") { init(); }

	void init(void);
};