#include <iostream>
#include <cmath>
#include <iomanip>

#include "debug.h"
#include "token.h"
#include "lexer.h"
#include "globals.h"
#include "ast.h"
#include "compiler.h"
#include "mathobj.h"

std::unordered_map<TokenType, const char *> tk_type_string =
{
	{ TokenType::T_IDENTIFIER,		"IDENTIFIER   " },

	{ TokenType::T_OPERATOR_SYM,	"OPERATOR_SYM " },

	{ TokenType::T_INTEGER_LITERAL,	"INTEGER      " },
	{ TokenType::T_REAL_LITERAL,	"REAL         " },
	
	{ TokenType::T_COMMA,			"COMMA        " },
	{ TokenType::T_SEMICOLON,		"SEMICOLON    " },
	{ TokenType::T_LEFT_PAREN,		"LEFT_PAREN   " },
	{ TokenType::T_RIGHT_PAREN,		"RIGHT_PAREN  " },
	{ TokenType::T_LEFT_BRACE,		"LEFT_BRACE   " },
	{ TokenType::T_RIGHT_BRACE,		"RIGHT_BRACE  " },
	{ TokenType::T_LEFT_BRACKET,	"LEFT_BRACKET " },
	{ TokenType::T_RIGHT_BRACKET,	"RIGHT_BRACKET" },

	{ TokenType::T_ERROR,			"ERROR        " },
	{ TokenType::T_EOF,				"EOF          " }
};

const char * tk_type_to_string(TokenType t_type)
{ return tk_type_string.find(t_type) != tk_type_string.end() ? tk_type_string[t_type] : "KEYWORD	  "; }

std::unordered_map<uint8_t, const char *> opcode_string =
{
	{ OpCode::OP_LOAD_CONST,	"LOAD_CONST "	},

	{ OpCode::OP_ENTER_BLOCK,	"ENTER_BLOCK"	},
	{ OpCode::OP_LEAVE_BLOCK,	"LEAVE_BLOCK"	},

	{ OpCode::OP_CALL_FUNCTION,	"CALL_FUNC  "	},
	{ OpCode::OP_LEAVE_FUNCTION,"LEAVE_FUNC "	},

	{ OpCode::OP_SET_VAR,		"SET_VAR    "	},
	{ OpCode::OP_LOAD_VAR,		"LOAD_VAR   "	},

	{ OpCode::OP_UNARY_OP,		"UNARY_OP   "	},
	{ OpCode::OP_BINARY_OP,		"BINARY_OP  "	},

	{ OpCode::OP_POP,			"POP        "	},
	{ OpCode::OP_RETURN,		"RETURN     "	},
	{ OpCode::OP_RETURN_VALUE,	"RETURN_VAL "	}
};

const char * opcode_to_string(uint8_t opcode)
{ return opcode_string[opcode]; }

void indent(int depth);

void Compiler::disassemble(void)
{
	for (auto & func : *functions)
	{
		if (func->type == FunctionType::F_BUILTIN)
			continue;
		auto custom_func = std::static_pointer_cast<CustomFunction>(func);
		disassemble(custom_func->chunk);
	}
	disassemble(chunk);
}

void Compiler::disassemble(std::shared_ptr<Chunk> & chunk)
{
	std::cout << chunk->name << ":\n";
	auto & bytes = chunk->bytecode();
	for (int i = 0; i < bytes.size(); i++)
	{
		std::cout << std::setw(2) << std::setfill('0') << std::hex;
		std::cout << (int)bytes[i] << " |\t"
			<< opcode_to_string(bytes[i]) << "\t";
		switch (bytes[i])
		{
			case OpCode::OP_LOAD_CONST:
				std::cout << (int)bytes[++i] << "\t\'";
				print_constant(chunk->constants[bytes[i]]);
				std::cout << "\'\n";
				break;

			case OpCode::OP_CALL_FUNCTION:
				std::cout << (int)bytes[++i] << "\t\'";
				print_function((*functions)[bytes[i]]);
				std::cout << "\'\n";
				break;
			
			case OpCode::OP_SET_VAR:
			case OpCode::OP_LOAD_VAR:
				std::cout << (int)bytes[++i] << "\t\'";
				print_variable((*variables)[bytes[i]]);
				std::cout << "\'\n";
				break;

			case OpCode::OP_UNARY_OP:
			case OpCode::OP_BINARY_OP:
				std::cout << (int)bytes[++i] << "\t\'";
				print_operator((*operators)[bytes[i]]);
				std::cout << "\'\n";
				break;

			default:
				std::cout << '\n';
				break;
		}
	}
	std::cout << '\n';
}

void Compiler::print_constant(std::shared_ptr<MathObj> & constant)
{
	if (constant->type().type == MOT::MO_REAL || constant->type().type == MOT::MO_INTEGER)
	{
		auto real = dynamic_cast<Real *>(constant.get());
		std::cout << real->value();
	}
	else if (constant->type().type == MOT::MO_VARIABLE)
	{
		auto * variable = constant->as<Variable>();
		std::cout << variable->name;
	}
	else
		std::cout << "ERROR";
}

void Compiler::print_variable(std::shared_ptr<Variable> & variable)
{
	std::cout << variable->name;
}

void Compiler::print_function(std::shared_ptr<Function> & function)
{
	std::cout << function->name;
}

void Compiler::print_operator(std::pair<std::shared_ptr<const OperatorFunction>, std::string> & op)
{
	std::cout << op.second;
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

void BlockNode::print(int depth) const
{
	indent(depth);
	std::cout << "Block :\n";
	for (auto & stmt : statements)
		stmt->print(depth + 1);
}

void ReturnNode::print(int depth) const
{
	indent(depth);
	std::cout << "Return\n";
}

void ReturnStatementNode::print(int depth) const
{
	indent(depth);
	std::cout << "Return Statement :\n";
	value->print(depth + 1);
}

void FunctionDeclarationNode::print(int depth) const
{
	indent(depth);
	std::cout << "Function Declaration :\n";
	name->print(depth + 1);
	for (auto & param : parameters)
		param->print(depth + 1);
	if (return_type)
		return_type->print(depth + 1);
	body->print(depth + 1);
}

void FunctionCallNode::print(int depth) const
{
	indent(depth);
	std::cout << "Function Call :\n";
	name->print(depth + 1);
	if (!arguments.empty())
	{
		indent(depth + 1);
		std::cout << "Arguments :\n";
		for (auto & arg : arguments)
			arg->print(depth + 2);
	}
}

void ParameterNode::print(int depth) const
{
	indent(depth);
	std::cout << "Parameter :\n";
	name->print(depth + 1);
	type->print(depth + 1);
	if (default_value)
		default_value->print(depth + 1);
}

void VariableDeclarationNode::print(int depth) const
{
	indent(depth);
	std::cout << "Variable Declaration :\n";
	type->print(depth + 1);
	name->print(depth + 1);
	if (value)
		value->print(depth + 1);
}

void ExpressionStatementNode::print(int depth) const
{
	indent(depth);
	std::cout << "Expression Statement :\n";
	for (auto & expr : expressions)
		expr->print(depth + 1);
}

void ExpressionNode::print(int depth) const
{
	indent(depth);
	std::cout << "Expression :\n";

	op->print(depth + 1);
	left->print(depth + 1);
	right->print(depth + 1);
}

void OperandNode::print(int depth) const
{
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
	indent(depth);
	std::cout << "Literal : " << value << '\n';
}

void IdentifierNode::print(int depth) const
{
	indent(depth);
	std::cout << "Identifier : " << name << '\n';
}

void OperatorNode::print(int depth) const
{
	indent(depth);
	std::cout << "Operator : " << op_info->name << '\n';
}

void TypeNode::print(int depth) const
{
	indent(depth);
	std::cout << "Type : ";
	//std::visit([&](auto & type) {
	//	using T = std::decay_t<decltype(type)>;
	//	if constexpr (std::is_same_v<T, MOT>)
	//		std::cout << type_to_string[type] <<'\n';
	//	else if constexpr (std::is_same_v<T, std::unique_ptr<IdentifierNode>>)
	//		std::cout << type->name << '\n';
	//}, type);
	std::cout << '\n';
}

void indent(int depth)
{
	for (int i = 0; i < depth; i++)
		std::cout << "    ";
}