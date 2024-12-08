#pragma once

#include "object/object.hpp"


class NoneObj : public MathObj
{
public:
	NoneObj() : MathObj(Builtins::mathobj_class) {}
	~NoneObj() = default;

	Type type(void) const override { return Type::MO_NONE; }
	std::string to_string(void) const override { return "none"; }

	MathObjPtr add(const MathObjPtr & rhs) const override { return MathObj::none; }
};