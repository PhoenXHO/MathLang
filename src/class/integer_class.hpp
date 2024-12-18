#pragma once

#include "class.hpp"

struct IntegerClass : public Class
{
	IntegerClass(void) : Class("Integer", { Builtins::mathobj_class }) {}

	void init(void) override;

	ObjectPtr instantiate(const std::any & value) const override;
	ObjectPtr default_value(void) const override;
	bool can_cast_to(const ClassPtr & cls) const override;
	ObjectPtr cast(const ObjectPtr & obj) const override;
};