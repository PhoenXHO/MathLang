#include <iostream>
#include <cmath>

#include "builtin.h"

// * Binary operators
BuiltinOpFunc ml__add__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	auto & lhs_integer = static_cast<const Real &>(lhs);
	auto & rhs_integer = static_cast<const Real &>(rhs);
	if (lhs.type() == MathObjType::MO_INTEGER && rhs.type() == MathObjType::MO_INTEGER)
	{
		return std::make_shared<Integer>(lhs_integer.value() + rhs_integer.value());
	}
	else
	{
		return std::make_shared<Real>(lhs_integer.value() + rhs_integer.value());
	}
};

BuiltinOpFunc ml__subtract__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	auto & lhs_integer = static_cast<const Real &>(lhs);
	auto & rhs_integer = static_cast<const Real &>(rhs);
	if (lhs.type() == MathObjType::MO_INTEGER && rhs.type() == MathObjType::MO_INTEGER)
	{
		return std::make_shared<Integer>(lhs_integer.value() - rhs_integer.value());
	}
	else
	{
		return std::make_shared<Real>(lhs_integer.value() - rhs_integer.value());
	}
};

BuiltinOpFunc ml__multiply__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	auto & lhs_integer = static_cast<const Real &>(lhs);
	auto & rhs_integer = static_cast<const Real &>(rhs);
	if (lhs.type() == MathObjType::MO_INTEGER && rhs.type() == MathObjType::MO_INTEGER)
	{
		return std::make_shared<Integer>(lhs_integer.value() * rhs_integer.value());
	}
	else
	{
		return std::make_shared<Real>(lhs_integer.value() * rhs_integer.value());
	}
};

BuiltinOpFunc ml__divide__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	auto & lhs_integer = static_cast<const Real &>(lhs);
	auto & rhs_integer = static_cast<const Real &>(rhs);
	if (lhs.type() == MathObjType::MO_INTEGER && rhs.type() == MathObjType::MO_INTEGER)
	{
		return std::make_shared<Integer>(lhs_integer.value() / rhs_integer.value());
	}
	else
	{
		return std::make_shared<Real>(lhs_integer.value() / rhs_integer.value());
	}
};

BuiltinOpFunc ml__exponentiate__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	auto & lhs_integer = static_cast<const Real &>(lhs);
	auto & rhs_integer = static_cast<const Real &>(rhs);
	if (lhs.type() == MathObjType::MO_INTEGER && rhs.type() == MathObjType::MO_INTEGER)
	{
		return std::make_shared<Integer>(std::pow(lhs_integer.value(), rhs_integer.value()));
	}
	else
	{
		return std::make_shared<Real>(std::pow(lhs_integer.value(), rhs_integer.value()));
	}
};

BuiltinOpFunc ml__assign__ = nullptr;

// * Unary operators
BuiltinOpFunc ml__negate__real = [](const MathObj & operand, const MathObj &) -> std::shared_ptr<MathObj>
{
	auto & operand_val = static_cast<const Real &>(operand);
	if (operand.type() == MathObjType::MO_INTEGER)
	{
		return std::make_shared<Integer>(-operand_val.value());
	}
	else
	{
		return std::make_shared<Real>(-operand_val.value());
	}
};

BuiltinOpFunc ml__print__real = [](const MathObj & operand, const MathObj &) -> std::shared_ptr<MathObj>
{
	std::cout << operand.to_string();
	return std::make_shared<None>();
};

BuiltinOpFunc ml__print__none = [](const MathObj &, const MathObj &) -> std::shared_ptr<MathObj>
{
	std::cout << "none";
	return std::make_shared<None>();
};