#include "semanalyzer.h"
#include "error.h"

std::unordered_map<MathObjType, std::string> mathobjtype_string =
{
	{ MathObjType::MO_NONE,		"None"		},
	{ MathObjType::MO_INTEGER,	"Integer"	},
	{ MathObjType::MO_REAL,		"Real"		}
};

std::string mathobjtype_to_string(MathObjType type)
{ return mathobjtype_string[type]; }

void SemanticAnalyzer::analyze_source(void)
{
	context_stack.push(std::make_pair(ContextType::C_GLOBAL, nullptr));
	for (const auto & statement : ast.statements)
	{
		panic_mode = false;
		analyze(statement.get());
	}
}

SemanticAnalyzer::AnalysisResult SemanticAnalyzer::analyze(ASTNode * node)
{
	if (panic_mode)
		return MathObjType::MO_NONE;

	switch (node->n_type)
	{
		case NodeType::N_BLOCK:
		{
			auto * block = static_cast<BlockNode *>(node);
			if (!scope->is_function_scope)
			{
				if (scope->children.size() >= UINT8_MAX)
					break;

				uint8_t index = scope->children.size();
				push_scope(scope);
				block->relative_index = index;
			}

			bool found_return = false;
			const FunctionDeclarationNode * func_decl = nullptr;

			for (const auto & statement : block->statements)
			{
				if ((statement->n_type == NodeType::N_RETURN || statement->n_type == NodeType::N_RETURN_STMT)
					&& !in_function())
				{
					register_semantic_error(
						"return statement outside of function body",
						"",
						statement.get()
					);
				}
				else if (in_function())
				{
					func_decl = static_cast<const FunctionDeclarationNode *>(context_stack.top().second);

					if (statement->n_type == NodeType::N_RETURN_STMT && func_decl->return_type->type == MathObjType::MO_NONE)
					{
						register_semantic_error(
							"non-returning function should not return a value",
							"",
							statement.get()
						);
					}
					else if (statement->n_type == NodeType::N_RETURN && func_decl->return_type->type != MathObjType::MO_NONE)
					{
						register_semantic_error(
							"returning function should return a value",
							"",
							statement.get()
						);
					}
					else if (statement->n_type == NodeType::N_RETURN_STMT)
					{
						found_return = true;
						auto * return_stmt = static_cast<ReturnStatementNode *>(statement.get());
						MathObjType expr_type = analyze(return_stmt->value.get()).type;
						if (!can_convert(expr_type, func_decl->return_type->type))
						{
							register_semantic_error(
								"cannot implicitly convert `" + mathobjtype_to_string(expr_type) + "` to `"+ mathobjtype_to_string(func_decl->return_type->type) + "`",
								"",
								return_stmt->value.get()
							);
						}
					}
					else
					{
						analyze(statement.get());
					}
				}
			}

			if (in_function() && func_decl->return_type->type != MathObjType::MO_NONE && !found_return)
			{
				register_semantic_error(
					"returning function should return a value",
					"",
					func_decl->name.get()
				);
			}

			if (!scope->is_function_scope)
				leave_scope(scope);
			break;
		}
		case NodeType::N_RETURN_STMT:
		{
			auto * return_stmt = static_cast<ReturnStatementNode *>(node);
			return analyze(return_stmt->value.get());
		}
		case NodeType::N_FUNC_DECL:
		{
			auto * func_decl = static_cast<FunctionDeclarationNode *>(node);
			auto & name = func_decl->name->name;

			context_stack.push(std::make_pair(
				func_decl->return_type->type == MathObjType::MO_NONE ?
					ContextType::C_NONRETURNING_FUNCTION : ContextType::C_RETURNING_FUNCTION,
				func_decl));

			// Look for a function with the same name and same arity and parameter types
			auto candidates = scope->get_function_implementations(std::string(name));
			if (!candidates.empty())
			{
				bool found_match = false;
				// Iterate over the candidate functions and check if any of them match the parameter types
				for (auto it = candidates.begin(); it != candidates.end(); ++it)
				{
					auto & func = it->second;
					if (func->parameters.size() != func_decl->parameters.size())
						continue;

					auto & params = func->parameters;

					bool match = true;
					// Check if the candidate function has the same parameter types as the function
					// parameters can have different names
					for (size_t i = 0; i < func_decl->parameters.size(); ++i)
					{
						if (func_decl->parameters[i]->type->type != params[i].second)
						{
							match = false;
							break;
						}
					}

					if (match)
					{
						found_match = true;
						break;
					}
				}

				if (found_match)
				{
					register_semantic_error(
						"function `" + std::string(name) + "` with the same parameter types is already defined in this scope",
						"",
						func_decl->name.get()
					);
					return MathObjType::MO_NONE;
				}
			}

			// Add the function to the list of functions
			auto function = std::make_shared<CustomFunction>(name, func_decl->return_type->type);
			scope->function_table.register_function(std::string(name), function);
			func_decl->function = function;

			if (scope->children.size() >= UINT8_MAX)
			{
				register_semantic_error(
					"too many scopes",
					"",
					node
				);
				return MathObjType::MO_NONE;
			}

			// Create a new scope for the function body
			push_scope(function->scope);
			function->scope->parent = scope;
			scope = function->scope;
			scope->is_function_scope = true;

			// Analyze the function parameters
			for (const auto & param : func_decl->parameters)
			{
				analyze(param.get());
				function->parameters.push_back(std::make_pair(std::string(param->name->name), param->type->type));
			}

			// Analyze the function body
			analyze(func_decl->body.get());

			leave_scope(scope);
			context_stack.pop();
			break;
		}
		case NodeType::N_FUNC_CALL:
		{
			auto * func_call = static_cast<FunctionCallNode *>(node);
			auto & name = func_call->name->name;

			// Look for a function with the same name and same types of arguments
			auto candidates = scope->get_function_implementations(std::string(name));
			auto candidates_copy = candidates;
			if (candidates.empty())
			{
				register_semantic_error(
					"function `" + std::string(name) + "` is not defined in this scope",
					"",
					func_call->name.get()
				);
				return MathObjType::MO_NONE;
			}

			std::vector<std::pair<std::shared_ptr<Function>, int>> matching_candidates;
			for (auto it = candidates.begin(); it != candidates.end(); ++it)
			{
				auto & func = it->second;
				if (func->parameters.size() != func_call->arguments.size())
					continue;

				auto & params = func->parameters;

				bool match = true;
				// Check if the candidate function has the same parameter types as the function
				// parameters can have different names
				for (size_t i = 0; i < func_call->arguments.size(); ++i)
				{
					MathObjType arg_type = analyze(func_call->arguments[i].get()).type;
					if (!can_convert(arg_type, params[i].second))
					{
						match = false;
						break;
					}
				}

				if (match)
				{
					int specificity = 0;
					for (size_t i = 0; i < func_call->arguments.size(); ++i)
					{
						MathObjType arg_type = analyze(func_call->arguments[i].get()).type;
						specificity += calculate_specificity(arg_type, params[i].second);
					}
					matching_candidates.push_back({ func, specificity });
				}
			}

			if (matching_candidates.empty())
			{
				std::string additional_info = "`";
				for (const auto & arg : func_call->arguments)
				{
					additional_info += mathobjtype_to_string(analyze(arg.get()).type) + "`, `";
				}
				additional_info.pop_back();
				additional_info.pop_back();
				additional_info.pop_back();

				if (!candidates_copy.empty())
				{
					additional_info += "\nCandidates are : \n";
					for (auto it = candidates_copy.begin(); it != candidates_copy.end(); ++it)
					{
						auto & func = it->second;
						additional_info += "  - " + func->name + "(";
						for (auto & param : func->parameters)
						{
							additional_info += mathobjtype_to_string(param.second) + ", ";
						}
						additional_info.pop_back();
						additional_info.pop_back();
						additional_info += ")\n";
					}
				}

				register_semantic_error(
					"no function `" + std::string(name) + "` matches these argument types",
					additional_info,
					func_call->name.get()
				);
				return MathObjType::MO_NONE;
			}

			// Sort the matching candidates by specificity
			std::sort(matching_candidates.begin(), matching_candidates.end(), [](auto & a, auto & b) {
				return a.second > b.second;
			});

			// Check if there's a unique maximum specificity
			if (matching_candidates.size() > 1 && matching_candidates[0].second == matching_candidates[1].second)
			{
				register_semantic_error(
					"ambiguous call to function `" + std::string(name) + "`",
					"",
					func_call->name.get()
				);
				return MathObjType::MO_NONE;
			}

			// Get the most specific candidate
			auto & candidate = matching_candidates[0];
			func_call->function = candidate.first;

			//auto it = scope->find_function(name);
			//if (it == scope->functions.end())
			//{
			//	register_semantic_error(
			//		"function `" + std::string(name) + "` is not defined in this scope",
			//		"",
			//		func_call->name.get()
			//	);
			//	return MathObjType::MO_NONE;
			//}

			// Check if the number of arguments matches the number of parameters
			//if (it->second->parameters.size() != func_call->arguments.size())
			//{
			//	std::string message = "function `" + std::string(name) + "` expects ";
			//	if (it->second->parameters.size() == 0)
			//		message += "no arguments";
			//	else if (it->second->parameters.size() == 1)
			//		message += "1 argument";
			//	else
			//		message += std::to_string(it->second->parameters.size()) + " arguments";
				
			//	message += ", but ";
			//	if (func_call->arguments.size() == 0)
			//		message += "none were provided";
			//	else if (func_call->arguments.size() == 1)
			//		message += "1 was provided";
			//	else
			//		message += std::to_string(func_call->arguments.size()) + " were provided";

			//	register_semantic_error(
			//		message,
			//		"",
			//		func_call->name.get()
			//	);
			//	return MathObjType::MO_NONE;
			//}

			// Analyze the function arguments
			for (const auto & arg : func_call->arguments)
			{
				analyze(arg.get());
			}

			return candidate.first->return_type;
		}
		case NodeType::N_PARAMETER:
		{
			auto * param = static_cast<ParameterNode *>(node);
			MathObjType param_type = param->type->type;

			if (param->default_value)
			{
				MathObjType expr_type = analyze(param->default_value.get()).type;
				if (!can_convert(expr_type, param_type))
				{
					register_semantic_error(
						"cannot implicitly convert `" + mathobjtype_to_string(expr_type) + "` to `" + mathobjtype_to_string(param_type) + "`",
						"",
						param->default_value.get()
					);
				}
			}
			
			// Add the parameter to the list of variables
			scope->variables[param->name->name] = std::make_shared<Variable>(param->name->name, param_type);

			return param_type;
		}
		case NodeType::N_EXPR_STMT:
		{
			auto * expr_stmt = static_cast<ExpressionStatementNode *>(node);
			for (const auto & expression : expr_stmt->expressions)
				analyze(expression.get());
			break;
		}
		case NodeType::N_VAR_DECL:
		{
			auto * var_decl = static_cast<VariableDeclarationNode *>(node);
			MathObjType var_type = var_decl->type->type;

			if (var_decl->value)
			{
				MathObjType expr_type = analyze(var_decl->value.get()).type;
				if (!can_convert(expr_type, var_type))
				{
					register_semantic_error(
						"cannot implicitly convert `" + mathobjtype_to_string(expr_type) + "` to `" + mathobjtype_to_string(var_type) + "`",
						"",
						var_decl->value.get()
					);
				}
			}

			// Look for a variable with the same name
			auto it = scope->find_variable(var_decl->name->name, in_function());
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
			auto * expr = static_cast<ExpressionNode *>(node);
			MathObjType left_type = analyze(expr->left.get()).type;
			MathObjType right_type = analyze(expr->right.get()).type;

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
					if (matching_candidates.size() > 1 && matching_candidates[0].second == matching_candidates[1].second)
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
						'`' + mathobjtype_to_string(left_type) + "`, `" + mathobjtype_to_string(right_type) + '`',
						op.get()
					);
				}
			}
			break;
		}
		case NodeType::N_OPERAND:
		{
			auto * operand = static_cast<OperandNode *>(node);
			MathObjType operand_type = analyze(operand->primary.get()).type;

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
				mathobjtype_to_string(operand_type),
				op.get()
			);
			break;
		}
		case NodeType::N_LITERAL:
		{
			auto * literal = static_cast<LiteralNode *>(node);
			return literal->type;
		}
		case NodeType::N_IDENTIFIER:
		{
			auto * identifier = static_cast<IdentifierNode *>(node);
			auto it = scope->find_variable(identifier->name);
			if (it == scope->variables.end())
			{
				register_semantic_error(
					"variable `" + std::string(identifier->name) + "` is not defined in this scope",
					"",
					identifier
				);
				break;
			}
			return it->second->value_type();
		}
	}
	return MathObjType::MO_NONE;
}

bool SemanticAnalyzer::in_function(void) const
{
	return context_stack.top().first == ContextType::C_RETURNING_FUNCTION || context_stack.top().first == ContextType::C_NONRETURNING_FUNCTION;
}

void SemanticAnalyzer::register_semantic_error(std::string message, std::string additional_info, const ASTNode * node)
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