#include "operator.h"
#include "builtinop.h"
#include <iostream>

std::pair<OpImplementations::const_iterator, OpImplementations::const_iterator> OperatorTable::get_implementations(std::string_view op_name, bool unary) const
{
	auto & implementations = unary ? unary_implemetations : binary_implemetations;

	std::string s_op_name(op_name);
	return implementations.equal_range(s_op_name);
}

std::shared_ptr<const Operator> OperatorTable::find(std::string_view op_name) const
{
	std::string s_op_name(op_name);
	auto it = operators.find(s_op_name);
	if (it != operators.end())
		return it->second;
	return nullptr;
}

void OperatorTable::register_operator(std::string name, Fixity fixity, Precedence precedence)
{
	operators.emplace(name, std::make_shared<Operator>(name, fixity, precedence));
}
void OperatorTable::register_unary_implementation(std::string name, BuiltinOpFunc & implementation, MathObjType arg_type, MathObjType ret_type)
{
	unary_implemetations.emplace(
		name,
		std::make_shared<OperatorFunction>(
			OperatorType::O_BUILTIN,
			implementation,
			std::make_pair(arg_type, MathObjType::MO_NONE),
			ret_type
		)
	);
}
void OperatorTable::register_binary_implementation(std::string name, BuiltinOpFunc & implementation, std::pair<MathObjType, MathObjType> arg_types, MathObjType ret_type)
{
	binary_implemetations.emplace(
		name,
		std::make_shared<OperatorFunction>(
			OperatorType::O_BUILTIN,
			implementation,
			arg_types, ret_type
		)
	);
}

void OperatorTable::register_builtin_operators(void)
{
	#define REG_OP register_operator

	#define REG_UN_IMP register_unary_implementation
	#define REG_BIN_IMP register_binary_implementation

	// Initial register
	REG_OP("+", Fixity::F_LEFT, Precedence::P_ADDITION);
	REG_OP("-", Fixity::F_LEFT, Precedence::P_ADDITION);
	REG_OP("*", Fixity::F_LEFT, Precedence::P_MULTIPLICATION);
	REG_OP("/", Fixity::F_LEFT, Precedence::P_MULTIPLICATION);
	REG_OP("^", Fixity::F_LEFT, Precedence::P_EXPONENTIATION);
	REG_OP("=", Fixity::F_RIGHT, Precedence::P_ASSIGNMENT);
	REG_OP("print", Fixity::F_LEFT, Precedence::P_UNARY);

	// Binary operators
	REG_BIN_IMP("+", ml__add__real_real, {MOT::MO_REAL, MOT::MO_REAL}, MOT::MO_REAL);
	REG_BIN_IMP("+", ml__add__real_real, {MOT::MO_INTEGER, MOT::MO_INTEGER}, MOT::MO_INTEGER);
	
	REG_BIN_IMP("-", ml__subtract__real_real, {MOT::MO_REAL, MOT::MO_REAL}, MOT::MO_REAL);
	REG_BIN_IMP("-", ml__subtract__real_real, {MOT::MO_INTEGER, MOT::MO_INTEGER}, MOT::MO_INTEGER);

	REG_BIN_IMP("*", ml__multiply__real_real, {MOT::MO_REAL, MOT::MO_REAL}, MOT::MO_REAL);
	REG_BIN_IMP("*", ml__multiply__real_real, {MOT::MO_INTEGER, MOT::MO_INTEGER}, MOT::MO_INTEGER);

	REG_BIN_IMP("/", ml__divide__real_real, {MOT::MO_REAL, MOT::MO_REAL}, MOT::MO_REAL);
	REG_BIN_IMP("/", ml__divide__real_real, {MOT::MO_INTEGER, MOT::MO_INTEGER}, MOT::MO_INTEGER);

	REG_BIN_IMP("^", ml__exponentiate__real_real, {MOT::MO_REAL, MOT::MO_REAL}, MOT::MO_REAL);
	REG_BIN_IMP("^", ml__exponentiate__real_real, {MOT::MO_INTEGER, MOT::MO_INTEGER}, MOT::MO_INTEGER);

	REG_BIN_IMP("=", ml__assign__real_real, {{MOT::MO_REAL, false}, MOT::MO_REAL}, MOT::MO_REAL);
	REG_BIN_IMP("=", ml__assign__real_real, {{MOT::MO_INTEGER, false}, MOT::MO_INTEGER}, MOT::MO_INTEGER);

	// Unary operators
	REG_UN_IMP("-", ml__negate__real, MOT::MO_REAL, MOT::MO_REAL);
	REG_UN_IMP("-", ml__negate__real, MOT::MO_INTEGER, MOT::MO_INTEGER);

	REG_UN_IMP("print", ml__print__real, MOT::MO_REAL, MOT::MO_NONE);
	REG_UN_IMP("print", ml__print__none, MOT::MO_NONE, MOT::MO_NONE);
}