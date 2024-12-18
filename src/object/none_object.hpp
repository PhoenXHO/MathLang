#pragma once

#include "object/object.hpp"


class NoneObj : public Object
{
public:
	NoneObj() : Object(Builtins::none_class) {}
	~NoneObj() = default;

	std::string to_string(void) const override
	{ return "none"; }

	ObjectPtr add(const ObjectPtr & rhs) const override
	{ return Object::none; }
	ObjectPtr cast_to(const ClassPtr & cls) override
	{ return nullptr; }
};