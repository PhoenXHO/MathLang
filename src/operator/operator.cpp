#include "operator.h"
#include "builtin.h"
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

void OperatorTable::register_operator(std::string name, std::shared_ptr<Operator> op)
{
	operators.insert({ name, std::move(op) });
}
void OperatorTable::register_un_implementation(std::string name, std::shared_ptr<OperatorFunction> op_func)
{
	unary_implemetations.insert({ name, std::move(op_func) });
}
void OperatorTable::register_bin_implementation(std::string name, std::shared_ptr<OperatorFunction> op_func)
{
	binary_implemetations.insert({ name, std::move(op_func) });
}

void OperatorTable::register_builtin_operators(void)
{
	#define REG_OP(name, fixity, precedence) \
		register_operator(name, std::make_shared<Operator>(name, fixity, precedence))

	#define REG_UN_IMP(name, implementation, type1, typ2, ret_type) \
		register_un_implementation(name, std::make_shared<OperatorFunction>(OperatorType::O_BUILTIN, implementation, std::make_pair<MathObjType, MathObjType>(type1, typ2), ret_type))
	#define REG_BIN_IMP(name, implementation, type1, typ2, ret_type) \
		register_bin_implementation(name, std::make_shared<OperatorFunction>(OperatorType::O_BUILTIN, implementation, std::make_pair<MathObjType, MathObjType>(type1, typ2), ret_type))

	// Initial register
	REG_OP("+", Fixity::F_LEFT, Precedence::P_ADDITION);
	REG_OP("-", Fixity::F_LEFT, Precedence::P_ADDITION);
	REG_OP("*", Fixity::F_LEFT, Precedence::P_MULTIPLICATION);
	REG_OP("/", Fixity::F_LEFT, Precedence::P_MULTIPLICATION);
	REG_OP("^", Fixity::F_LEFT, Precedence::P_EXPONENTIATION);
	REG_OP(":=", Fixity::F_RIGHT, Precedence::P_ASSIGNMENT);
	REG_OP("print", Fixity::F_LEFT, Precedence::P_UNARY);

	// Binary operators
	REG_BIN_IMP("+", ml__add__real_real, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);
	REG_BIN_IMP("+", ml__add__real_real, MathObjType::MO_INTEGER, MathObjType::MO_INTEGER, MathObjType::MO_INTEGER);
	
	REG_BIN_IMP("-", ml__subtract__real_real, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);
	REG_BIN_IMP("-", ml__subtract__real_real, MathObjType::MO_INTEGER, MathObjType::MO_INTEGER, MathObjType::MO_INTEGER);

	REG_BIN_IMP("*", ml__multiply__real_real, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);
	REG_BIN_IMP("*", ml__multiply__real_real, MathObjType::MO_INTEGER, MathObjType::MO_INTEGER, MathObjType::MO_INTEGER);

	REG_BIN_IMP("/", ml__divide__real_real, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);
	REG_BIN_IMP("/", ml__divide__real_real, MathObjType::MO_INTEGER, MathObjType::MO_INTEGER, MathObjType::MO_INTEGER);

	REG_BIN_IMP("^", ml__exponentiate__real_real, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);
	REG_BIN_IMP("^", ml__exponentiate__real_real, MathObjType::MO_INTEGER, MathObjType::MO_INTEGER, MathObjType::MO_INTEGER);

	REG_BIN_IMP(":=", ml__assign__, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);

	// Unary operators
	REG_UN_IMP("-", ml__negate__real, MathObjType::MO_REAL, MathObjType::MO_NONE, MathObjType::MO_REAL);
	REG_UN_IMP("-", ml__negate__real, MathObjType::MO_INTEGER, MathObjType::MO_NONE, MathObjType::MO_INTEGER);

	REG_UN_IMP("print", ml__print__real, MathObjType::MO_REAL, MathObjType::MO_NONE, MathObjType::MO_NONE);
	REG_UN_IMP("print", ml__print__real, MathObjType::MO_INTEGER, MathObjType::MO_NONE, MathObjType::MO_NONE);
	REG_UN_IMP("print", ml__print__none, MathObjType::MO_NONE, MathObjType::MO_NONE, MathObjType::MO_NONE);
}