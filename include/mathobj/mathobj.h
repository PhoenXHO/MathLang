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
		MO_INTEGER = 1,
		MO_REAL = 2 * MO_INTEGER
	};

	virtual ~MathObj() = default;
	virtual bool operator==(const MathObj & other) const = 0;

	virtual std::string to_string(void) const = 0;
	virtual MathObjType type(void) const = 0;
	virtual std::shared_ptr<MathObj> default_value(void) const = 0;
};
typedef MathObj::MathObjType MathObjType;

inline bool can_convert(MathObjType from, MathObjType to)
{ return from == to || 2 * from == to; }

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

	std::shared_ptr<MathObj> default_value(void) const override
	{ return std::make_shared<Real>(0.0); }
};

struct IntegerValue : public Real
{
	IntegerValue(int value) : Real(value) {}
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

	std::shared_ptr<MathObj> default_value(void) const override
	{ return std::make_shared<IntegerValue>(0); }
};

struct NoneValue : public MathObj
{
	bool operator==(const MathObj & other) const override
	{ throw std::runtime_error("invalid comparison with `none`"); }

	std::string to_string(void) const override
	{ return "none"; }

	MathObjType type(void) const override
	{ return MathObjType::MO_NONE; }

	std::shared_ptr<MathObj> default_value(void) const override
	{ return std::make_shared<NoneValue>(); }
};

#endif // MATHOBJ_H