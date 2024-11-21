#include "operator/operator.hpp"

std::shared_ptr<Operator> OperatorTable::find(std::string_view symbol, bool is_unary) const
{
	auto & operators = is_unary ? unary_operators : binary_operators;

	auto it = operators.find(std::string(symbol));
	if (it != operators.end())
	{
		return it->second;
	}

	return nullptr;
}

std::shared_ptr<Operator> OperatorTable::register_binary_operator(const std::string & symbol, Associativity associativity, Precedence precedence)
{
	return binary_operators.insert({ symbol, std::make_unique<Operator>(symbol, Fixity::F_INFIX, associativity, precedence) }).first->second;
}

std::shared_ptr<Operator> OperatorTable::register_unary_operator(const std::string & symbol, Fixity fixity)
{
	return unary_operators.insert({ symbol, std::make_unique<Operator>(symbol, fixity, Associativity::A_NONE, Precedence::P_UNARY) }).first->second;
}

void OperatorTable::register_builtin_operators(void)
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
	//TODO: Implement implicit type conversion
	add->add_implementation(
		MathObj::Type::MO_REAL,
		MathObj::Type::MO_REAL,
		std::make_shared<BuiltinOperatorImplentation>(
			[](const MathObjPtr & lhs, const MathObjPtr & rhs) -> MathObjPtr
			{
				return lhs->add(rhs);
			},
			MathObj::Type::MO_REAL
		)
	);
	add->add_implementation(
		MathObj::Type::MO_INTEGER,
		MathObj::Type::MO_INTEGER,
		std::make_shared<BuiltinOperatorImplentation>(
			[](const MathObjPtr & lhs, const MathObjPtr & rhs) -> MathObjPtr
			{
				return lhs->add(rhs);
			},
			MathObj::Type::MO_INTEGER
		)
	);
}