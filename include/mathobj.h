#ifndef MATHOBJ_H
#define MATHOBJ_H

#include <string>

struct MathObj
{
	enum class MathObjType
	{
		MO_INTEGER,
		MO_REAL,
		MO_NONE
	};

	virtual ~MathObj() = default;
	virtual bool operator==(const MathObj & other) const = 0;

	virtual std::string to_string(void) const = 0;
	virtual MathObjType type(void) const = 0;
};
typedef MathObj::MathObjType MathObjType;

struct RealValue : public MathObj
{
protected:
	double _value_;

public:
	RealValue(double value) : _value_(value) {}
	bool operator==(const MathObj & other) const override
	{
		if (other.type() != MathObjType::MO_REAL)
			return false;
		return _value_ == static_cast<const RealValue &>(other).value();
	}

	std::string to_string(void) const override
	{ return std::to_string(_value_); }

	MathObjType type(void) const override
	{ return MathObjType::MO_REAL; }

	double value(void) const
	{ return _value_; }
};

struct IntegerValue : public RealValue
{
	IntegerValue(int value) : RealValue(value) {}
	bool operator==(const MathObj & other) const override
	{
		if (other.type() != MathObjType::MO_INTEGER)
			return false;
		return _value_ == static_cast<const IntegerValue &>(other).value();
	}

	std::string to_string(void) const override
	{ return std::to_string((int)_value_); }

	MathObjType type(void) const override
	{ return MathObjType::MO_INTEGER; }

	int value(void) const
	{ return _value_; }
};

struct NoneValue : public MathObj
{
	bool operator==(const MathObj & other) const override
	{ throw std::runtime_error("invalid comparison with `none`"); }

	std::string to_string(void) const override
	{ return "none"; }

	MathObjType type(void) const override
	{ return MathObjType::MO_NONE; }
};

#endif // MATHOBJ_H