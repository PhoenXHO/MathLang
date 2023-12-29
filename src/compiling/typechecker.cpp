#include "typechecker.h"
#include "error.h"
#include <iostream>

std::unordered_map<MathObjType, std::string> type_to_string =
{
	{ MathObjType::MO_NONE,		"None"		},
	{ MathObjType::MO_INTEGER,	"Integer"	},
	{ MathObjType::MO_REAL,		"Real"		}
};

void TypeChecker::check_types(void)
{
	for (const auto & statement : ast.statements)
	{
		panic_mode = false;
		check_types(statement);
	}
}

MathObjType TypeChecker::check_types(const std::unique_ptr<ASTNode> & node)
{
	if (panic_mode)
		return MathObjType::MO_NONE;

	switch (node->n_type)
	{
		case NodeType::N_EXPR_STMT:
		{
			auto * expr_stmt = static_cast<ExpressionStatementNode *>(node.get());
			for (const auto & expression : expr_stmt->expressions)
				check_types(expression);
			break;
		}
		case NodeType::N_VAR_DECL:
		{
			auto * var_decl = static_cast<VariableDeclarationNode *>(node.get());
			MathObjType var_type = var_decl->type->type;

			if (var_decl->expression)
			{
				MathObjType expr_type = check_types(var_decl->expression);
				if (!can_convert(expr_type, var_type))
				{
					register_semantic_error(
						"cannot implicitly convert `" + type_to_string[expr_type] + "` to `" + type_to_string[var_type] + "`",
						"",
						var_decl->expression.get()
					);
				}
			}

			// Add the variable to the list of variables
			variables[var_decl->name->name] = std::make_tuple(var_type, std::make_shared<NoneValue>());

			return var_type;
		}
		case NodeType::N_EXPR:
		{
			auto * expr = static_cast<ExpressionNode *>(node.get());
			MathObjType left_type = check_types(expr->left);
			MathObjType right_type = check_types(expr->right);

			auto & op = expr->op;

			// Get the list of implementations for the operator
			auto candidates = operator_table->get_implementations(op->op_info->name);
			if (candidates.first != candidates.second)
			{
				// Iterate over the candidate operators and check if any of them match the operand types
				for (auto & it = candidates.first; it != candidates.second; ++it)
				{
					auto arg_types = it->second->arg_types;
					if (can_convert(left_type, arg_types.first) && can_convert(right_type, arg_types.second))
					{
						op->op_func = it->second.get();
						return it->second->return_type;
					}
				}
				// No matching operator found
			}
			
			register_semantic_error(
				"no binary operator `" + op->op_info->name + "` matches these operand types",
				type_to_string[left_type] + "`, `" + type_to_string[right_type],
				op.get()
			);
			break;
		}
		case NodeType::N_OPERAND:
		{
			auto * operand = static_cast<OperandNode *>(node.get());
			MathObjType operand_type = check_types(operand->primary);

			auto & op = operand->op;
			if (!op)
				return operand_type;
			
			auto candidates = operator_table->get_implementations(op->op_info->name, true);
			if (candidates.first != candidates.second)
			{
				// Iterate over the candidate operators and check if any of them match the operand type
				for (auto & it = candidates.first; it != candidates.second; ++it)
				{
					auto arg_type = it->second->arg_types.first;
					if (can_convert(operand_type, arg_type))
					{
						op->op_func = it->second.get();
						return it->second->return_type;
					}
				}
				// No matching operator found
			}

			register_semantic_error(
				"no unary operator `" + op->op_info->name + "` matches this operand type",
				type_to_string[operand_type],
				op.get()
			);
			break;
		}
		case NodeType::N_LITERAL:
		{
			auto * literal = static_cast<LiteralNode *>(node.get());
			return literal->type;
		}
	}
	return MathObjType::MO_NONE;
}

void TypeChecker::register_semantic_error(std::string message, std::string additional_info, const ASTNode * node)
{
	if (panic_mode)
		return;
	panic_mode = true;

	std::unique_ptr<Error> err { new SemanticError(
		message,
		additional_info,
		node->line,
		node->column,
		node->start_position,
		node->end_position - node->start_position
	)};
	ErrorHandler::push_error(err);
}