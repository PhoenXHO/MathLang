#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <string>
#include <memory>

struct Symbol
{
	enum class Type
	{
		S_NONE,
		S_VARIABLE
		// For now, we only have variables
	};

	static const std::shared_ptr<Symbol> empty_symbol;

	Symbol(std::string_view name, Type type, std::shared_ptr<void> value)
		: name(name), type(type), value(value) {}

	std::string_view get_name(void) const
	{ return name; }
	Type get_type(void) const
	{ return type; }
	const std::shared_ptr<void> & get_value(void) const
	{ return value; }

private:
	std::string name;
	Type type;
protected:
	friend class Scope;
	std::shared_ptr<void> value; // The value can be a MathObjPtr, FunctionPtr, or ClassPtr, depending on the type
};

#endif // SYMBOL_HPP