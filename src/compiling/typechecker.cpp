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
		case NodeType::N_BLOCK:
		{
			if (scope->children.size() >= UINT8_MAX)
			{
				register_semantic_error(
					"too many nested scopes",
					"",
					node.get()
				);
				return MathObjType::MO_NONE;
			}

			uint8_t index = scope->children.size();
			push_scope(scope);
			auto * block = static_cast<BlockNode *>(node.get());
			block->relative_index = index;

			for (const auto & statement : block->statements)
				check_types(statement);
			leave_scope(scope);
			break;
		}
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

			if (var_decl->value)
			{
				MathObjType expr_type = check_types(var_decl->value);
				if (!can_convert(expr_type, var_type))
				{
					register_semantic_error(
						"cannot implicitly convert `" + type_to_string[expr_type] + "` to `" + type_to_string[var_type] + "`",
						"",
						var_decl->value.get()
					);
				}
			}

			// Look for a variable with the same name
			auto it = scope->find_variable(var_decl->name->name);
			if (it != scope->variables.end())
			{
				register_semantic_error(
					"variable `" + std::string(var_decl->name->name) + "` is already defined in this scope",
					"",
					var_decl->name.get()
				);
				return var_type;
			}
			
			// Add the variable to the list of variables
			scope->variables[var_decl->name->name] = std::make_shared<Variable>(var_decl->name->name, var_type);

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
				std::vector<std::pair<std::shared_ptr<OperatorFunction>, int>> matching_candidates;

				// Iterate over the candidate operators and check if any of them match the operand types
				for (auto & it = candidates.first; it != candidates.second; ++it)
				{
					auto arg_types = it->second->arg_types;
					int specificity = calculate_specificity(left_type, right_type, arg_types.first, arg_types.second);

					if (specificity > 0)
						matching_candidates.push_back({ it->second, specificity });
				}
				
				if (!matching_candidates.empty())
				{
					// Sort the matching candidates by specificity
					std::sort(matching_candidates.begin(), matching_candidates.end(), [](auto & a, auto & b) {
						return a.second > b.second;
					});

					// Check if there's a unique maximum specificity
					if (matching_candidates[0].second == matching_candidates[1].second)
					{
						register_semantic_error(
							"ambiguous call to operator `" + op->op_info->name + "`",
							"",
							op.get()
						);
						return MathObjType::MO_NONE;
					}

					// Get the most specific candidate
					auto & candidate = matching_candidates[0];
					op->op_func = candidate.first;
					return candidate.first->return_type;

					// No matching operator found
					register_semantic_error(
						"no binary operator `" + op->op_info->name + "` matches these operand types",
						type_to_string[left_type] + "`, `" + type_to_string[right_type],
						op.get()
					);
				}
			}
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
						op->op_func = it->second;
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
		case NodeType::N_IDENTIFIER:
		{
			auto * identifier = static_cast<IdentifierNode *>(node.get());
			auto it = scope->find_variable(identifier->name);
			if (it == scope->variables.end())
			{
				register_semantic_error(
					"variable `" + std::string(identifier->name) + "` is not defined in this scope",
					"",
					identifier
				);
				return MathObjType::MO_NONE;
			}
			return it->second->value_type();
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