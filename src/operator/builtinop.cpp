#include <iostream>
#include <cmath>

#include "builtinop.h"

// * Binary operators
BuiltinOpFunc ml__add__real_real = [](std::shared_ptr<MathObj> & lhs, std::shared_ptr<MathObj> & rhs) -> std::shared_ptr<MathObj>
{
	auto lhs_value = std::static_pointer_cast<Real>(get_value(lhs));
	auto rhs_value = std::static_pointer_cast<Real>(get_value(rhs));
	if (lhs->type().type == MOT::MO_INTEGER && rhs->type().type == MOT::MO_INTEGER)
	{
		return std::make_shared<Integer>(lhs_value->value() + rhs_value->value());
	}
	else
	{
		return std::make_shared<Real>(lhs_value->value() + rhs_value->value());
	}
};

BuiltinOpFunc ml__subtract__real_real = [](std::shared_ptr<MathObj> & lhs, std::shared_ptr<MathObj> & rhs) -> std::shared_ptr<MathObj>
{
	auto lhs_value = std::static_pointer_cast<Real>(get_value(lhs));
	auto rhs_value = std::static_pointer_cast<Real>(get_value(rhs));
	if (lhs->type().type == MOT::MO_INTEGER && rhs->type().type == MOT::MO_INTEGER)
	{
		return std::make_shared<Integer>(lhs_value->value() - rhs_value->value());
	}
	else
	{
		return std::make_shared<Real>(lhs_value->value() - rhs_value->value());
	}
};

BuiltinOpFunc ml__multiply__real_real = [](std::shared_ptr<MathObj> & lhs, std::shared_ptr<MathObj> & rhs) -> std::shared_ptr<MathObj>
{
	auto lhs_value = std::static_pointer_cast<Real>(get_value(lhs));
	auto rhs_value = std::static_pointer_cast<Real>(get_value(rhs));
	if (lhs->type().type == MOT::MO_INTEGER && rhs->type().type == MOT::MO_INTEGER)
	{
		return std::make_shared<Integer>(lhs_value->value() * rhs_value->value());
	}
	else
	{
		return std::make_shared<Real>(lhs_value->value() * rhs_value->value());
	}
};

BuiltinOpFunc ml__divide__real_real = [](std::shared_ptr<MathObj> & lhs, std::shared_ptr<MathObj> & rhs) -> std::shared_ptr<MathObj>
{
	auto lhs_value = std::static_pointer_cast<Real>(get_value(lhs));
	auto rhs_value = std::static_pointer_cast<Real>(get_value(rhs));
	if (lhs->type().type == MOT::MO_INTEGER && rhs->type().type == MOT::MO_INTEGER)
	{
		return std::make_shared<Integer>(lhs_value->value() / rhs_value->value());
	}
	else
	{
		return std::make_shared<Real>(lhs_value->value() / rhs_value->value());
	}
};

BuiltinOpFunc ml__exponentiate__real_real = [](std::shared_ptr<MathObj> & lhs, std::shared_ptr<MathObj> & rhs) -> std::shared_ptr<MathObj>
{
	auto lhs_value = std::static_pointer_cast<Real>(get_value(lhs));
	auto rhs_value = std::static_pointer_cast<Real>(get_value(rhs));
	if (lhs->type().type == MOT::MO_INTEGER && rhs->type().type == MOT::MO_INTEGER)
	{
		return std::make_shared<Integer>(std::pow(lhs_value->value(), rhs_value->value()));
	}
	else
	{
		return std::make_shared<Real>(std::pow(lhs_value->value(), rhs_value->value()));
	}
};

BuiltinOpFunc ml__assign__real_real = [](std::shared_ptr<MathObj> & lhs, std::shared_ptr<MathObj> & rhs) -> std::shared_ptr<MathObj>
{
	if (lhs->type().type == MOT::MO_VARIABLE)
	{
		auto & lhs_val = static_cast<Variable &>(*lhs);
		lhs_val.value = rhs;
		return rhs;
	}
	else
	{
		// just in case of a bug
		throw std::runtime_error("invalid assignment");
	}
};

// * Unary operators
BuiltinOpFunc ml__negate__real = [](std::shared_ptr<MathObj> & operand, std::shared_ptr<MathObj> &) -> std::shared_ptr<MathObj>
{
	auto & operand_val = static_cast<const Real &>(*get_value(operand));
	if (operand->type().type == MOT::MO_INTEGER)
	{
		return std::make_shared<Integer>(-operand_val.value());
	}
	else
	{
		return std::make_shared<Real>(-operand_val.value());
	}
};

BuiltinOpFunc ml__print__real = [](std::shared_ptr<MathObj> &, std::shared_ptr<MathObj> & operand) -> std::shared_ptr<MathObj>
{
	auto operand_val = std::static_pointer_cast<Real>(get_value(operand));
	std::cout << operand_val->to_string();
	return std::make_shared<None>();
};

BuiltinOpFunc ml__print__none = [](std::shared_ptr<MathObj> &, std::shared_ptr<MathObj> & operand) -> std::shared_ptr<MathObj>
{
	std::cout << "none";
	return std::make_shared<None>();
};