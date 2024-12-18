#pragma once

#include "class.hpp"

struct NoneClass : public Class
{
	NoneClass(void) : Class("None") {}

	void init(void) override;

	ObjectPtr instantiate(const std::any & value) const override;
	ObjectPtr default_value(void) const override;
	bool can_cast_to(const ClassPtr & cls) const override;
	ObjectPtr cast(const ObjectPtr & obj) const override;
};