#include <iostream>
#include <cmath>
#include <iomanip>

#include "debug.h"
#include "token.h"
#include "lexer.h"
#include "globals.h"
#include "ast.h"
#include "compiler.h"

std::unordered_map<Type, const char *> type_to_string =
{
	{ Type::T_INTEGER,	"T_INTEGER" },
	{ Type::T_REAL,		"T_REAL"	}
};
std::unordered_map<uint8_t, const char *> opcode_to_string =
{
	{ OpCode::OP_LOAD_CONST,	"LOAD_CONST"	},

	{ OpCode::OP_UNARY_OP,		"UNARY_OP"		},
	{ OpCode::OP_BINARY_OP,		"BINARY_OP"		},

	{ OpCode::OP_POP,			"POP"			},
	{ OpCode::OP_RETURN,		"RETURN"		}
};

void indent(int depth);

void Compiler::print_bytecode(void)
{
	for (int i = 0; i < bytes.size(); i++)
	{
		std::cout << std::setw(2) << std::setfill('0') << std::hex;
		switch (bytes[i])
		{
			case OpCode::OP_LOAD_CONST:
				std::cout << (int)bytes[i] << " |\t"
					<< opcode_to_string[bytes[i]] << '\t'
					<< (int)bytes[++i] << "\t\'";
				print_constant(constants[bytes[i]]);
				std::cout << "\'\n";
				break;

			case OpCode::OP_UNARY_OP:
			case OpCode::OP_BINARY_OP:
				std::cout << (int)bytes[i] << " |\t"
					<< opcode_to_string[bytes[i]] << '\t'
					<< (int)bytes[++i] << "\t\'";
				print_operator(operators[bytes[i]]);
				std::cout << "\'\n";
				break;

			case OpCode::OP_POP:
			case OpCode::OP_RETURN:
				std::cout << (int)bytes[i] << " |\t"
					<< opcode_to_string[bytes[i]] << '\n';
				break;
		}
	}
}

void Compiler::print_constant(std::shared_ptr<MathObj> & constant)
{
	if (constant->type() == MathObjType::MO_REAL || constant->type() == MathObjType::MO_INTEGER)
	{
		auto real = dynamic_cast<RealValue *>(constant.get());
		std::cout << real->value();
	}
}

void Compiler::print_operator(const Operator * op)
{
	std::cout << op->name;
}

void d_print_token(std::unique_ptr<Token> & token)
{
	static size_t prev_line = 0;
	
	if (prev_line != token->line())
		std::cout << token->line();
	else
		std::cout << '|';

	std::cout << '\t' << tk_type_to_string(token->type()) << " \t";
	if (token->lexeme() != "")
		std::cout << '\"' << token->lexeme() << '\"';
	std::cout << '\n';

	prev_line = token->line();
}

void AST::print(void) const
{
	std::cout << "Program :\n";
	for (auto & stmt : statements)
		stmt->print(1);
}

void VariableDeclarationNode::print(int depth) const
{
	if (!this)
		return;
	indent(depth);
	std::cout << "Variable Declaration :\n";
	type->print(depth + 1);
	name->print(depth + 1);
	expression->print(depth + 1);
}

void ExpressionStatementNode::print(int depth) const
{
	if (!this)
		return;
	indent(depth);
	std::cout << "Expression Statement :\n";
	for (auto & expr : expressions)
		expr->print(depth + 1);
}

void ExpressionNode::print(int depth) const
{
	if (!this)
		return;
	indent(depth);
	std::cout << "Expression :\n";

	left->print(depth + 1);
	op->print(depth + 1);
	right->print(depth + 1);
}

void OperandNode::print(int depth) const
{
	if (!this)
		return;
	indent(depth);
	std::cout << "Operand :\n";

	if (op)
		op->print(depth + 1);

	if (primary)
		primary->print(depth + 1);
	else
	{
		indent(depth + 1);
		std::cout << "ERROR\n";
	}
}

void LiteralNode::print(int depth) const
{
	if (!this)
		return;
	indent(depth);
	std::cout << "Literal : " << value << '\n';
}

void IdentifierNode::print(int depth) const
{
	if (!this)
		return;
	indent(depth);
	std::cout << "Identifier : " << name << '\n';
}

void OperatorNode::print(int depth) const
{
	if (!this)
		return;
	indent(depth);
	std::cout << "Operator : " << op_sym << '\n';
}

void TypeNode::print(int depth) const
{
	if (!this)
		return;
	indent(depth);
	std::cout << "Type : ";
	std::visit([&](auto & type) {
		using T = std::decay_t<decltype(type)>;
		if constexpr (std::is_same_v<T, Type>)
			std::cout << type_to_string[type] <<'\n';
		else if constexpr (std::is_same_v<T, std::unique_ptr<IdentifierNode>>)
			std::cout << type->name << '\n';
	}, type);
}

void indent(int depth)
{
	for (int i = 0; i < depth; i++)
		std::cout << "    ";
}