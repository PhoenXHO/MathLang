#pragma once

#include "class.hpp"

struct ReferenceClass : public Class
{
	ReferenceClass(void) : Class("Reference", { Builtins::mathobj_class }) {}

	void init(void) override;

	ObjectPtr instantiate(const std::any & value) const override;
	ObjectPtr default_value(void) const override;
	bool can_cast_to(const ClassPtr & cls) const override;
	ObjectPtr cast(const ObjectPtr & obj) const override;

	std::string to_string(void) const override
	{
		std::ostringstream oss;
		oss << "<Reference>";
		return oss.str();
	}
};