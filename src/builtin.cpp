#include <iostream>
#include <cmath>

#include "builtin.h"

// * Binary operators
OperatorFunction ml__add__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	if ((lhs.type() == MathObjType::MO_INTEGER || lhs.type() == MathObjType::MO_REAL)
	 && (rhs.type() == MathObjType::MO_INTEGER || rhs.type() == MathObjType::MO_REAL))
	{
		auto & lhs_integer = static_cast<const RealValue &>(lhs);
		auto & rhs_integer = static_cast<const RealValue &>(rhs);
		return std::make_shared<RealValue>(lhs_integer.value() + rhs_integer.value());
	}
	else
		throw std::runtime_error("invalid types for operator `+`");
};

OperatorFunction ml__subtract__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	if ((lhs.type() == MathObjType::MO_INTEGER || lhs.type() == MathObjType::MO_REAL)
	 && (rhs.type() == MathObjType::MO_INTEGER || rhs.type() == MathObjType::MO_REAL))
	{
		auto & lhs_integer = static_cast<const RealValue &>(lhs);
		auto & rhs_integer = static_cast<const RealValue &>(rhs);
		return std::make_shared<RealValue>(lhs_integer.value() - rhs_integer.value());
	}
	else
		throw std::runtime_error("invalid types for operator `-`");
};

OperatorFunction ml__multiply__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	if ((lhs.type() == MathObjType::MO_INTEGER || lhs.type() == MathObjType::MO_REAL)
	 && (rhs.type() == MathObjType::MO_INTEGER || rhs.type() == MathObjType::MO_REAL))
	{
		auto & lhs_integer = static_cast<const RealValue &>(lhs);
		auto & rhs_integer = static_cast<const RealValue &>(rhs);
		return std::make_shared<RealValue>(lhs_integer.value() * rhs_integer.value());
	}
	else
		throw std::runtime_error("invalid types for operator `*`");
};

OperatorFunction ml__divide__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	if ((lhs.type() == MathObjType::MO_INTEGER || lhs.type() == MathObjType::MO_REAL)
	 && (rhs.type() == MathObjType::MO_INTEGER || rhs.type() == MathObjType::MO_REAL))
	{
		auto & lhs_integer = static_cast<const RealValue &>(lhs);
		auto & rhs_integer = static_cast<const RealValue &>(rhs);
		return std::make_shared<RealValue>(lhs_integer.value() / rhs_integer.value());
	}
	else
		throw std::runtime_error("invalid types for operator `/`");
};

OperatorFunction ml__exponentiate__real_real = [](const MathObj & lhs, const MathObj & rhs) -> std::shared_ptr<MathObj>
{
	if ((lhs.type() == MathObjType::MO_INTEGER || lhs.type() == MathObjType::MO_REAL)
	 && (rhs.type() == MathObjType::MO_INTEGER || rhs.type() == MathObjType::MO_REAL))
	{
		auto & lhs_integer = static_cast<const RealValue &>(lhs);
		auto & rhs_integer = static_cast<const RealValue &>(rhs);
		return std::make_shared<RealValue>(std::pow(lhs_integer.value(), rhs_integer.value()));
	}
	else
		throw std::runtime_error("invalid types for operator `^`");
};

OperatorFunction ml__assign__ = nullptr;

// * Unary operators
OperatorFunction ml__negate__real = [](const MathObj & operand, const MathObj &) -> std::shared_ptr<MathObj>
{
	if (operand.type() == MathObjType::MO_INTEGER || operand.type() == MathObjType::MO_REAL)
	{
		auto & real = static_cast<const RealValue &>(operand);
		return std::make_shared<RealValue>(-real.value());
	}
	else
		throw std::runtime_error("invalid type for operator `-`");
};

OperatorFunction ml__print__real = [](const MathObj & operand, const MathObj &) -> std::shared_ptr<MathObj>
{
	std::cout << operand.to_string();
	return std::make_shared<NoneValue>();
};