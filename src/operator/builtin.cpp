#include <iostream>
#include <cmath>

#include "builtin.h"

// * Binary operators
BuiltinOpFunc ml__add__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	if ((lhs.type() == MathObjType::MO_INTEGER || lhs.type() == MathObjType::MO_REAL)
	 && (rhs.type() == MathObjType::MO_INTEGER || rhs.type() == MathObjType::MO_REAL))
	{
		auto & lhs_integer = static_cast<const Real &>(lhs);
		auto & rhs_integer = static_cast<const Real &>(rhs);
		return std::make_shared<Real>(lhs_integer.value() + rhs_integer.value());
	}
	else
		throw std::runtime_error("invalid types for operator `+`");
};

BuiltinOpFunc ml__subtract__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	if ((lhs.type() == MathObjType::MO_INTEGER || lhs.type() == MathObjType::MO_REAL)
	 && (rhs.type() == MathObjType::MO_INTEGER || rhs.type() == MathObjType::MO_REAL))
	{
		auto & lhs_integer = static_cast<const Real &>(lhs);
		auto & rhs_integer = static_cast<const Real &>(rhs);
		return std::make_shared<Real>(lhs_integer.value() - rhs_integer.value());
	}
	else
		throw std::runtime_error("invalid types for operator `-`");
};

BuiltinOpFunc ml__multiply__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	if ((lhs.type() == MathObjType::MO_INTEGER || lhs.type() == MathObjType::MO_REAL)
	 && (rhs.type() == MathObjType::MO_INTEGER || rhs.type() == MathObjType::MO_REAL))
	{
		auto & lhs_integer = static_cast<const Real &>(lhs);
		auto & rhs_integer = static_cast<const Real &>(rhs);
		return std::make_shared<Real>(lhs_integer.value() * rhs_integer.value());
	}
	else
		throw std::runtime_error("invalid types for operator `*`");
};

BuiltinOpFunc ml__divide__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	if ((lhs.type() == MathObjType::MO_INTEGER || lhs.type() == MathObjType::MO_REAL)
	 && (rhs.type() == MathObjType::MO_INTEGER || rhs.type() == MathObjType::MO_REAL))
	{
		auto & lhs_integer = static_cast<const Real &>(lhs);
		auto & rhs_integer = static_cast<const Real &>(rhs);
		return std::make_shared<Real>(lhs_integer.value() / rhs_integer.value());
	}
	else
		throw std::runtime_error("invalid types for operator `/`");
};

BuiltinOpFunc ml__exponentiate__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	if ((lhs.type() == MathObjType::MO_INTEGER || lhs.type() == MathObjType::MO_REAL)
	 && (rhs.type() == MathObjType::MO_INTEGER || rhs.type() == MathObjType::MO_REAL))
	{
		auto & lhs_integer = static_cast<const Real &>(lhs);
		auto & rhs_integer = static_cast<const Real &>(rhs);
		return std::make_shared<Real>(std::pow(lhs_integer.value(), rhs_integer.value()));
	}
	else
		throw std::runtime_error("invalid types for operator `^`");
};

BuiltinOpFunc ml__assign__ = nullptr;

// * Unary operators
BuiltinOpFunc ml__negate__real = [](const MathObj & operand, const MathObj &) -> std::shared_ptr<MathObj>
{
	if (operand.type() == MathObjType::MO_INTEGER || operand.type() == MathObjType::MO_REAL)
	{
		auto & real = static_cast<const Real &>(operand);
		return std::make_shared<Real>(-real.value());
	}
	else
		throw std::runtime_error("invalid type for operator `-`");
};

BuiltinOpFunc ml__print__real = [](const MathObj & operand, const MathObj &) -> std::shared_ptr<MathObj>
{
	std::cout << operand.to_string();
	return std::make_shared<NoneValue>();
};

BuiltinOpFunc ml__print__none = [](const MathObj &, const MathObj &) -> std::shared_ptr<MathObj>
{
	std::cout << "none";
	return std::make_shared<NoneValue>();
};