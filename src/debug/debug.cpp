#include <iostream>
#include <iomanip> // for `std::setw` and `std::setfill`

#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "compiler/chunk.hpp"
#include "util/config.hpp"


// Function to indent the output
void indent(int indent);


void Chunk::disassemble(void) const
{
	if (!config::dev)
	{
		return;
	}

	std::cout << "=== Chunk '" << name << "' ===\n";

	for (auto ip = code.cbegin(); ip != code.cend();)
	{
		ip = disassemble_instruction(ip);
		std::cout << std::setw(0) << std::setfill(' ') << '\n';
	}
}

std::vector<uint8_t>::const_iterator Chunk::disassemble_instruction(std::vector<uint8_t>::const_iterator ip) const
{
	std::cout << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
			  << static_cast<int>(*ip) << ' '
			  << std::dec << std::setw(20) << std::setfill(' ') << std::left;
	switch (static_cast<OpCode>(*ip))
	{
	case OP_LOAD_CONSTANT:
		std::cout << "LOAD_CONSTANT"
				  << std::setw(6) << std::setfill(' ') << std::right
				  << static_cast<int>(*(ip + 1)) << std::left
				  << "  (" << constant_pool[static_cast<int>(*(ip + 1))]->to_string() << ')';
		return ip + 2;

	case OP_SET_VARIABLE:
	{
		auto variable = current_scope->get_variable(static_cast<int>(*(ip + 1)));
		std::cout << "SET_VARIABLE" 
				  << std::setw(6) << std::setfill(' ') << std::right
				  << static_cast<int>(*(ip + 1)) << std::left
				  << "  " << variable->to_string();
		return ip + 2;
	}
	case OP_GET_VARIABLE:
	{
		auto variable = current_scope->get_variable(static_cast<int>(*(ip + 1)));
		std::cout << "GET_VARIABLE" 
				  << std::setw(6) << std::setfill(' ') << std::right
				  << static_cast<int>(*(ip + 1)) << std::left
				  << "  " << variable->to_string();
		return ip + 2;
	}

	case OP_CALL_FUNCTION:
		std::cout << "CALL_FUNCTION"
				  << std::setw(6) << std::setfill(' ') << std::right
				  << static_cast<int>(*(ip + 1)) << std::left
				  << "  " << static_cast<int>(*(ip + 2))
				  << "  '" << current_scope->get_function(static_cast<int>(*(ip + 1)))->to_string() << '\'';
		return ip + 3;

	case OP_CALL_UNARY:
		std::cout << "CALL_UNARY"
				  << std::setw(6) << std::setfill(' ') << std::right
				  << static_cast<int>(*(ip + 1)) << std::left;
		return ip + 2;
	case OP_CALL_BINARY:
		std::cout << "CALL_BINARY"
				  << std::setw(6) << std::setfill(' ') << std::right
				  << static_cast<int>(*(ip + 1)) << std::left;
		return ip + 2;

	case OP_PRINT:
		std::cout << "PRINT";
		return ip + 1;

	case OP_POP:
		std::cout << "POP";
		return ip + 1;
	case OP_RETURN:
		std::cout << "RETURN";
		return ip + 1;

	default:
		std::cout << "__UNKNOWN__";
		return ip + 1;
	}
}

// A map to convert token types to strings
static std::unordered_map<Token::Type, std::string_view> token_type_to_string
{
	{ Token::Type::T_ERROR,           "ERROR" },
	{ Token::Type::T_EOL,             "EOL" },
	{ Token::Type::T_EOF,             "EOF" },

	{ Token::Type::T_LET,             "LET" },
	{ Token::Type::T_DEFINE,          "DEFINE" },
	{ Token::Type::T_CLASS,           "CLASS" },
	{ Token::Type::T_IN,              "IN" },
	{ Token::Type::T_IF,              "IF" },
	{ Token::Type::T_ELSE,            "ELSE" },
	{ Token::Type::T_ITERATE,         "ITERATE" },
	{ Token::Type::T_SUM,             "SUM" },
	{ Token::Type::T_PROD,            "PROD" },
	{ Token::Type::T_USE,             "USE" },
	{ Token::Type::T_OPERATOR,        "OPERATOR" },
	{ Token::Type::T_NONE,            "NONE" },

	{ Token::Type::T_OPERATOR_SYMBOL,    "OPERATOR_SYM" },

	{ Token::Type::T_MATHOBJ,         "MATHOBJ" },
	{ Token::Type::T_NATURAL,         "NATURAL" },
	{ Token::Type::T_INTEGER,         "INTEGER" },
	{ Token::Type::T_RATIONAL,        "RATIONAL" },
	{ Token::Type::T_REAL,            "REAL" },
	{ Token::Type::T_COMPLEX,         "COMPLEX" },
	{ Token::Type::T_BOOLEAN,         "BOOLEAN" },
	{ Token::Type::T_SET,             "SET" },
	{ Token::Type::T_LIST,            "LIST" },
	{ Token::Type::T_VECTOR,          "VECTOR" },
	{ Token::Type::T_STRING,          "STRING" },
	{ Token::Type::T_CHAR,            "CHAR" },

	{ Token::Type::T_IDENTIFIER,      "IDENTIFIER" },

	{ Token::Type::T_INTEGER_LITERAL, "INTEGER_LITERAL" },
	{ Token::Type::T_REAL_LITERAL,    "REAL_LITERAL" },
	{ Token::Type::T_I,               "I" },
	{ Token::Type::T_STRING_LITERAL,  "STRING_LITERAL" },
	{ Token::Type::T_CHAR_LITERAL,    "CHAR_LITERAL" },
	{ Token::Type::T_SET_LITERAL,     "SET_LITERAL" },
	{ Token::Type::T_LIST_LITERAL,    "LIST_LITERAL" },
	{ Token::Type::T_VECTOR_LITERAL,  "VECTOR_LITERAL" },
	{ Token::Type::T_TRUE,            "TRUE" },
	{ Token::Type::T_FALSE,           "FALSE" },

	{ Token::Type::T_COMMA,           "COMMA" },
	{ Token::Type::T_SEMICOLON,       "SEMICOLON" },
	{ Token::Type::T_DOT,             "DOT" },
	{ Token::Type::T_LEFT_PAREN,      "LEFT_PAREN" },
	{ Token::Type::T_RIGHT_PAREN,     "RIGHT_PAREN" },
	{ Token::Type::T_LEFT_BRACKET,    "LEFT_BRACKET" },
	{ Token::Type::T_RIGHT_BRACKET,   "RIGHT_BRACKET" },
	{ Token::Type::T_LEFT_BRACE,      "LEFT_BRACE" },
	{ Token::Type::T_RIGHT_BRACE,     "RIGHT_BRACE" },
	{ Token::Type::T_ARROW,           "ARROW" },
	{ Token::Type::T_COLON,           "COLON" },
	{ Token::Type::T_COLON_ARROW,     "COLON_ARROW" },
	{ Token::Type::T_COLON_EQUAL,     "COLON_EQUAL" },
	{ Token::Type::T_RETURN,          "RETURN" }
};

std::ostream & operator<<(std::ostream & os, const Token & tk)
{
	if (!config::dev)
	{
		return os;
	}

	// Disable 0-fill
	os << std::setfill(' ');

	// Format: <line>:<column> <type> <lexeme>
	os << std::right << std::setw(4) << tk.location().line // <line> (4 characters, right-aligned)
	   << ':'
	   << std::left << std::setw(4) << tk.location().column // <column> (4 characters, left-aligned)
	   << ' ' << std::setw(16) << Token::type_to_string(tk.type()); // <type> (16 characters, left-aligned)

	if (tk.type() != Token::Type::T_EOL && tk.type() != Token::Type::T_EOF)
	{
		os << std::right << std::setw(16) << tk.lexeme(); // <lexeme> (16 characters, right-aligned)
	}

	os << '\n';
	return os;
}

std::string Token::type_to_string(Token::Type type)
{
	auto it = token_type_to_string.find(type);
	if (it != token_type_to_string.end())
	{
		return std::string(it->second);
	}
	return "UNKNOWN_TOKEN";
}

std::ostream & operator<<(std::ostream & os, const AST & ast)
{
	if (!config::dev)
	{
		return os;
	}

	os << "Program:\n";
	for (const auto &stmt : ast.statements)
	{
		if (stmt)
			stmt->print(1);
	}
	os << '\n';

	return os;
}

void VariableDeclarationNode::print(int depth) const
{
	indent(depth);
	std::cout << "Variable Declaration:\n";

	indent(depth + 1);
	std::cout << "Identifier: " << identifier->name << '\n';

	if (expression)
	{
		expression->print(depth + 1);
	}
}

void ExpressionStatementNode::print(int depth) const
{
	indent(depth);
	std::cout << "Expression Statement:\n";

	if (expression)
	{
		expression->print(depth + 1);
	}
	else
	{
		indent(depth + 1);
		std::cout << "ERROR\n";
	}
}

void ExpressionNode::print(int depth) const
{
	indent(depth);
	std::cout << "Expression:\n";

	left->print(depth + 1);
	op->print(depth + 1);
	right->print(depth + 1);
}

void OperandNode::print(int depth) const
{
	indent(depth);
	std::cout << "Operand:\n";

	if (op)
	{
		op->print(depth + 1);
	}
	
	if (primary)
	{
		primary->print(depth + 1);
	}
	else
	{
		indent(depth + 1);
		std::cout << "ERROR\n";
	}
}

void OperatorNode::print(int depth) const
{
	indent(depth);
	std::cout << "Operator: " << op->symbol() << '\n';
}

void FunctionCallNode::print(int depth) const
{
	indent(depth);
	std::cout << "Function Call:\n";

	identifier->print(depth + 1);

	for (const auto & arg : arguments)
	{
		arg->print(depth + 1);
	}
}

void IdentifierNode::print(int depth) const
{
	indent(depth);
	std::cout << "Identifier: " << name << '\n';
}

void TypeNode::print(int depth) const
{
	indent(depth);
	std::cout << "Type: " << name << '\n';
}

void LiteralNode::print(int depth) const
{
	indent(depth);
	std::cout << "Literal: " << value << ' ' << cls->to_string() << '\n';
}

void indent(int indent)
{
	std::string indent_str;
	for (int i = 0; i < indent; i++)
	{
		indent_str += "    ";
	}
	std::cout << indent_str;
}