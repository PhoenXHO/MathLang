#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include "symbol/symbol.hpp"
#include "object/object.hpp"

struct Variable : public Symbol
{
	Variable(std::string_view name, MathObjPtr & value)
		: Symbol(name, Symbol::Type::S_VARIABLE, value) {}

	const MathObjPtr get_value(void) const
	{
		return std::static_pointer_cast<MathObj>(value);
	}
};

class Reference : public MathObj
{
	std::shared_ptr<Symbol> symbol;
	
public:
	Reference(const Symbol & symbol) : symbol(std::make_shared<Symbol>(symbol)) {}
	Reference(std::shared_ptr<Symbol> symbol) : symbol(symbol) {}
	~Reference() = default;
	
	MathObj::Type type(void) const override { return MathObj::Type::MO_REFERENCE; }
	std::string to_string(void) const override
	{
		std::ostringstream oss;
		oss << '<' << symbol->get_name() << '(' << symbol.get() << ")>";
		return oss.str();
	}
	
	MathObjPtr add(const MathObjPtr & rhs) const override { return nullptr; }
};

#endif // VARIABLE_HPP