#include "operator.h"
#include "builtin.h"

const Operator * OperatorTable::get_opinfo(std::string_view op_name, bool unary) const
{
	auto & operators = unary ? unary_operators : binary_operators;

	std::string s_op_name(op_name);
	auto it = operators.find(s_op_name);
	if (it != operators.end())
		return it->second.get();
	return nullptr;
}

void OperatorTable::register_un_operator(std::string name, std::unique_ptr<Operator> op_info)
{ unary_operators[name] = std::move(op_info); }
void OperatorTable::register_bin_operator(std::string name, std::unique_ptr<Operator> op_info)
{ binary_operators[name] = std::move(op_info); }

void OperatorTable::register_builtin_operators(void)
{
	#define REG_UN_OP(name, fixity, precedence, implementation, type1, typ2, ret_type) \
		register_un_operator(name, std::make_unique<BuiltinOperator>(name, fixity, precedence, implementation, std::make_pair<MathObjType, MathObjType>(type1, typ2), ret_type))
	#define REG_BIN_OP(name, fixity, precedence, implementation, type1, typ2, ret_type) \
		register_bin_operator(name, std::make_unique<BuiltinOperator>(name, fixity, precedence, implementation, std::make_pair<MathObjType, MathObjType>(type1, typ2), ret_type))

	// Binary operators
	REG_BIN_OP("+", Fixity::F_LEFT, Precedence::P_ADDITION, ml__add__real_real, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);
	REG_BIN_OP("-", Fixity::F_LEFT, Precedence::P_ADDITION, ml__subtract__real_real, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);
	REG_BIN_OP("*", Fixity::F_LEFT, Precedence::P_MULTIPLICATION, ml__multiply__real_real, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);
	REG_BIN_OP("/", Fixity::F_LEFT, Precedence::P_MULTIPLICATION, ml__divide__real_real, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);
	REG_BIN_OP("^", Fixity::F_LEFT, Precedence::P_EXPONENTIATION, ml__exponentiate__real_real, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);
	REG_BIN_OP(":=", Fixity::F_RIGHT, Precedence::P_ASSIGNMENT, ml__assign__, MathObjType::MO_REAL, MathObjType::MO_REAL, MathObjType::MO_REAL);

	// Unary operators
	REG_UN_OP("-", Fixity::F_LEFT, Precedence::P_ADDITION, ml__negate__real, MathObjType::MO_REAL, MathObjType::MO_NONE, MathObjType::MO_REAL);
	REG_UN_OP("print", Fixity::F_LEFT, Precedence::P_UNARY, ml__print__real, MathObjType::MO_REAL, MathObjType::MO_NONE, MathObjType::MO_NONE);
}