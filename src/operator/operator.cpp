#include "operator/operator.hpp"


const OperatorPtr OperatorRegistry::find(std::string_view symbol, bool is_unary) const
{
	auto & operators = is_unary ? unary_operators : binary_operators;
	return operators[symbol];
}

OperatorPtr OperatorRegistry::register_binary_operator(const std::string & symbol, Associativity associativity, Precedence precedence)
{
	return binary_operators.define(symbol, std::make_shared<Operator>(symbol, Fixity::F_INFIX, associativity, precedence));
	//return binary_operators.insert({ symbol, std::make_unique<Operator>(symbol, Fixity::F_INFIX, associativity, precedence) }).first->second;
}

OperatorPtr OperatorRegistry::register_unary_operator(const std::string & symbol, Fixity fixity)
{
	return unary_operators.define(symbol, std::make_shared<Operator>(symbol, fixity, Associativity::A_NONE, Precedence::P_UNARY));
	//return unary_operators.insert({ symbol, std::make_unique<Operator>(symbol, fixity, Associativity::A_NONE, Precedence::P_UNARY) }).first->second;
}

void OperatorRegistry::register_builtin_operators(void)
{
	////* Binary operators
	//register_binary_operator("+", Associativity::A_LEFT, Precedence::P_ADDITION);
	//register_binary_operator("-", Associativity::A_LEFT, Precedence::P_ADDITION);
	//register_binary_operator("*", Associativity::A_LEFT, Precedence::P_MULTIPLICATION);
	//register_binary_operator("/", Associativity::A_LEFT, Precedence::P_MULTIPLICATION);
	//register_binary_operator("%", Associativity::A_LEFT, Precedence::P_MULTIPLICATION);
	//register_binary_operator("^", Associativity::A_RIGHT, Precedence::P_EXPONENTIATION);
	//register_binary_operator("=", Associativity::A_RIGHT, Precedence::P_ASSIGNMENT);

	////* Unary operators
	//register_unary_operator("+", Fixity::F_PREFIX);
	//register_unary_operator("-", Fixity::F_PREFIX);
	//register_unary_operator("!", Fixity::F_POSTFIX);

	//* Binary operators
	auto add = register_binary_operator("+", Associativity::A_LEFT, Precedence::P_ADDITION);
	auto sub = register_binary_operator("-", Associativity::A_LEFT, Precedence::P_ADDITION);
	auto mul = register_binary_operator("*", Associativity::A_LEFT, Precedence::P_MULTIPLICATION);
	auto div = register_binary_operator("/", Associativity::A_LEFT, Precedence::P_MULTIPLICATION);
	auto mod = register_binary_operator("%", Associativity::A_LEFT, Precedence::P_MULTIPLICATION);
	auto exp = register_binary_operator("^", Associativity::A_RIGHT, Precedence::P_EXPONENTIATION);
	
	//* Unary operators
	auto uplus = register_unary_operator("+", Fixity::F_PREFIX);
	auto uminus = register_unary_operator("-", Fixity::F_PREFIX);
	auto bang = register_unary_operator("!", Fixity::F_POSTFIX);



	// Implementations
	add->add_implementation(
		Builtins::real_class,
		Builtins::real_class,
		std::make_shared<BuiltinOperatorImplentation>(
			[](const MathObjPtr & lhs, const MathObjPtr & rhs) -> MathObjPtr
			{
				return lhs->add(rhs);
			},
			Builtins::real_class
		)
	);
	add->add_implementation(
		Builtins::integer_class,
		Builtins::integer_class,
		std::make_shared<BuiltinOperatorImplentation>(
			[](const MathObjPtr & lhs, const MathObjPtr & rhs) -> MathObjPtr
			{
				return lhs->add(rhs);
			},
			Builtins::integer_class
		)
	);
}