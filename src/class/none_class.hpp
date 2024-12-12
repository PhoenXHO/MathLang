#pragma once

#include "class.hpp"

struct NoneClass : public Class
{
	NoneClass(void) : Class("None") { init(); }

	void init(void);
};