#pragma once

#include "object/object.hpp"


class NoneObj : public MathObj
{
public:
	NoneObj() : MathObj(Builtins::none_class) {}
	~NoneObj() = default;

	std::string to_string(void) const override
	{ return "none"; }

	MathObjPtr add(const MathObjPtr & rhs) const override
	{ return MathObj::none; }
	MathObjPtr cast_to(const ClassPtr & cls) override
	{ return nullptr; }
};