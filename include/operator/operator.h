#ifndef OPERATOR_H
#define OPERATOR_H

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>

#include "token.h"
#include "mathobj.h"

struct Operator;
struct OperatorFunction;
using BuiltinOpFunc = std::function<std::shared_ptr<MathObj> (std::shared_ptr<MathObj> &, std::shared_ptr<MathObj> &)>;
using OpImplementations = std::unordered_multimap<std::string, std::shared_ptr<OperatorFunction>>;
using Operators = std::unordered_multimap<std::string, std::shared_ptr<Operator>>;

// Fixity of an operator (left or right)
enum class Fixity
{
	F_LEFT, // Left fixity
	F_RIGHT // Right fixity
};

// Precedence of an operator
enum Precedence
{
	P_MIN = 0, // Minimum precedence
	P_ASSIGNMENT,
	P_ADDITION,
	P_MULTIPLICATION,
	P_EXPONENTIATION,
	P_UNARY, // Unary operators
};

struct Operator
{
	enum class OperatorType
	{
		O_BUILTIN, O_CUSTOM
	};

	Operator(
		std::string name,
		Fixity fixity,
		Precedence precedence
	) :
		name(name),
		fixity(fixity),
		precedence(precedence)
	{}
	virtual ~Operator() = default;

	std::string name;
	Fixity fixity;
	Precedence precedence;

	// for debugging
	std::string_view get_name(void) const
	{ return name; }
};
typedef Operator::OperatorType OperatorType;

// Implementation of a builtin operator.
struct OperatorFunction
{
	OperatorFunction(
		OperatorType type,
		BuiltinOpFunc implementation,
		std::pair<MathObjType, MathObjType> arg_types,
		MathObjType return_type,
		// Reverse order of arguments because unary operators only have one argument (the right one)
		bool rhs_is_const = true,
		bool lhs_is_const = true
	) :
		type(type),
		implementation(implementation),
		arg_types(arg_types),
		return_type(return_type)
	{}

	OperatorType type;
	BuiltinOpFunc implementation;
	std::pair<MathObjType, MathObjType> arg_types;
	MathObjType return_type;
};

// Table of operators
class OperatorTable
{
	// TODO: Remove Operators from here (the compiler doesn't need it)
	Operators operators; // Operator information without implementations (fixity and precedence)
	OpImplementations binary_implemetations; // Binary operator implementations
	OpImplementations unary_implemetations; // Unary operator implementations

public:
	friend class Compiler; // Compiler needs access to the operator table
	
	void register_builtin_operators(void);
	void register_operator(std::string name, Fixity fixity, Precedence precedence);
	void register_unary_implementation(std::string name, BuiltinOpFunc & implementation, MathObjType arg_type, MathObjType ret_type);
	void register_binary_implementation(std::string name, BuiltinOpFunc & implementation, std::pair<MathObjType, MathObjType> arg_types, MathObjType ret_type);

	// Find an operator by name
	std::shared_ptr<const Operator> find(std::string_view op_name) const;
	// Find an operator implementation by name
	std::pair<OpImplementations::const_iterator, OpImplementations::const_iterator> get_implementations(std::string_view op_name, bool unary = false) const;
};

#endif // OPERATOR_H