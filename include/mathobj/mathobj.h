#ifndef MATHOBJ_H
#define MATHOBJ_H

#include <string>
#include <memory>

struct  MathObj
{
	// A type A is convertible to type B if and only if B == 2 * A or B == A.
	enum MathObjType
	{
		MO_NONE = -1,
		MO_VARIABLE = 0,
		MO_INTEGER = 1,
		MO_REAL = 2 * MO_INTEGER
	};

	virtual ~MathObj() = default;
	virtual bool operator==(const MathObj & other) const = 0;

	virtual std::string to_string(void) const = 0;
	virtual MathObjType type(void) const = 0;

	template<typename T>
	T * as(void)
	{ return dynamic_cast<T *>(this); }
};
typedef MathObj::MathObjType MathObjType;

inline bool can_convert(MathObjType from, MathObjType to)
{ return from == to || 2 * from == to; }

inline int calculate_specificity(MathObjType left, MathObjType right, MathObjType argt_l, MathObjType argt_r)
{
	int specificity = 0;
	
	if (left == argt_l)
		specificity += 2;
	else if (can_convert(left, argt_l))
		specificity += 1;

	if (right == argt_r)
		specificity += 2;
	else if (can_convert(right, argt_r))
		specificity += 1;

	return specificity;
}

struct Real : public MathObj
{
protected:
	double _value_;

public:
	Real(double value) : _value_(value) {}
	bool operator==(const MathObj & other) const override
	{
		if (other.type() != MathObjType::MO_REAL)
			return false;
		return _value_ == static_cast<const Real &>(other).value();
	}

	std::string to_string(void) const override
	{ return std::to_string(_value_); }

	MathObjType type(void) const override
	{ return MathObjType::MO_REAL; }

	double value(void) const
	{ return _value_; }
};

struct Integer : public Real
{
	Integer(int value) : Real(value) {}
	bool operator==(const MathObj & other) const override
	{
		if (other.type() != MathObjType::MO_INTEGER)
			return false;
		return _value_ == static_cast<const Integer &>(other).value();
	}

	std::string to_string(void) const override
	{ return std::to_string((int)_value_); }

	MathObjType type(void) const override
	{ return MathObjType::MO_INTEGER; }

	int value(void) const
	{ return _value_; }
};

struct None : public MathObj
{
	bool operator==(const MathObj & other) const override
	{ throw std::runtime_error("invalid comparison with `None`"); }

	std::string to_string(void) const override
	{ return "none"; }

	MathObjType type(void) const override
	{ return MathObjType::MO_NONE; }
};

struct Variable : public MathObj
{
	std::string name;
	MathObjType _value_type_;
	std::shared_ptr<MathObj> value;

	Variable(std::string_view name, MathObjType type) :
		name(name),
		_value_type_(type),
		value(std::shared_ptr<None>())
	{}
	Variable(std::string name) : Variable(name, MathObjType::MO_NONE) {}
	bool operator==(const MathObj & other) const override
	{ return *value.get() == other; }

	std::string to_string(void) const override
	{ return name; }

	MathObjType type(void) const override
	{ return MathObjType::MO_VARIABLE; }

	MathObjType value_type(void) const
	{ return _value_type_; }
};

#endif // MATHOBJ_H