#ifndef OPERATOR_H
#define OPERATOR_H

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>

#include "token.h"
#include "mathobj.h"

using OperatorFunction = std::function<std::shared_ptr<MathObj> (const MathObj &, const MathObj &)>;

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
		OperatorType type,
		Fixity fixity,
		Precedence precedence,
		std::pair<MathObjType, MathObjType> arg_types,
		MathObjType return_type
	) :
		name(name),
		type(type),
		fixity(fixity),
		precedence(precedence),
		arg_types(arg_types),
		return_type(return_type)
	{}
	virtual ~Operator() = default;

	std::string name;
	OperatorType type;
	Fixity fixity;
	Precedence precedence;
	std::pair<MathObjType, MathObjType> arg_types;
	MathObjType return_type;
};
typedef Operator::OperatorType OperatorType;

struct BuiltinOperator : Operator
{
	BuiltinOperator(
		std::string name,
		Fixity fixity,
		Precedence precedence,
		OperatorFunction implementation,
		std::pair<MathObjType, MathObjType> arg_types,
		MathObjType return_type
	) :
		Operator(name, OperatorType::O_BUILTIN, fixity, precedence, arg_types, return_type),
		implementation(implementation)
	{}

	OperatorFunction implementation;
	OperatorFunction & apply = implementation;
};

class OperatorTable
{
	std::unordered_map<std::string, std::unique_ptr<Operator>> binary_operators;
	std::unordered_map<std::string, std::unique_ptr<Operator>> unary_operators;

public:
	friend class Compiler;
	
	void register_builtin_operators(void);

	void register_un_operator(std::string name, std::unique_ptr<Operator> op_info);
	void register_bin_operator(std::string name, std::unique_ptr<Operator> op_info);

	const Operator * get_opinfo (std::string_view op_name, bool unary = false) const;
};

#endif // OPERATOR_H