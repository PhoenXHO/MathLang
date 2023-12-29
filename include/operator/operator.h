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
using BuiltinOpFunc = std::function<std::shared_ptr<MathObj> (const MathObj &, const MathObj &)>;
using OpImplementations = std::unordered_multimap<std::string, std::unique_ptr<OperatorFunction>>;
using Operators = std::unordered_multimap<std::string, std::unique_ptr<Operator>>;

enum class Fixity
{
	F_LEFT,
	F_RIGHT
};

enum Precedence
{
	P_MIN = 0,
	P_ASSIGNMENT,
	P_ADDITION,
	P_MULTIPLICATION,
	P_EXPONENTIATION,
	P_UNARY
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
};
typedef Operator::OperatorType OperatorType;

struct OperatorFunction
{
	OperatorFunction(
		OperatorType type,
		BuiltinOpFunc implementation,
		std::pair<MathObjType, MathObjType> arg_types,
		MathObjType return_type
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

class OperatorTable
{
	Operators operators;
	OpImplementations binary_implemetations;
	OpImplementations unary_implemetations;

public:
	friend class Compiler;
	
	void register_builtin_operators(void);

	void register_operator(std::string name, std::unique_ptr<Operator> op);
	void register_un_implementation(std::string name, std::unique_ptr<OperatorFunction> op_func);
	void register_bin_implementation(std::string name, std::unique_ptr<OperatorFunction> op_func);

	const Operator * find(std::string_view op_name) const;
	std::pair<OpImplementations::const_iterator, OpImplementations::const_iterator> get_implementations(std::string_view op_name, bool unary = false) const;
};

#endif // OPERATOR_H