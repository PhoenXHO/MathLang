#ifndef MATHOBJ_H
#define MATHOBJ_H

#include <string>
#include <memory>
#include <unordered_map>

// A type A is convertible to type B if and only if B == 2 * A or B == A.
struct MathObjType
{
	enum MOType
	{
		MO_NONE = -1,
		MO_VARIABLE = 0,
		MO_INTEGER = 1,
		MO_REAL = 2 * MO_INTEGER
	};

	MOType type;
	bool is_const;

	MathObjType() = default;
	MathObjType(MOType type, bool is_const = true) : type(type), is_const(is_const) {}
};
typedef MathObjType::MOType MOT;

struct  MathObj
{
	MathObjType _type_;

	MathObj(MOT type, bool is_const = true) : _type_(type, is_const) {}
	virtual ~MathObj() = default;

	virtual std::string to_string(void) const = 0;
	bool is_const(void) const
	{ return _type_.is_const; }
	MathObjType type(void) const
	{ return _type_; }

	template<typename T>
	T * as(void)
	{ return static_cast<T *>(this); }
};

inline bool can_convert(MathObjType & from, MathObjType & to)
{
	return from.type == to.type || to.type == 2 * from.type;
}

inline int calculate_specificity(MathObjType & type, MathObjType & argt)
{
	int specificity = 0;
	// Calculate specificity based on type and constness
	if (type.type == argt.type)
		specificity += 2;
	else if (can_convert(type, argt))
		specificity += 1;

	if (type.is_const == argt.is_const)
		specificity += 2;
	else if (type.is_const && !argt.is_const)
		specificity += 1;

	return specificity;
}

inline int calculate_specificity(MathObjType & left, MathObjType & right, MathObjType & argt_l, MathObjType & argt_r)
{
	int specificity = 0;
	
	specificity += calculate_specificity(left, argt_l);
	specificity += calculate_specificity(right, argt_r);

	return specificity;
}

struct Real : public MathObj
{
protected:
	double _value_;

public:
	Real(double value, MOT type = MOT::MO_REAL) :
		MathObj(type),
		_value_(value)
	{}

	std::string to_string(void) const override
	{ return std::to_string(_value_); }

	double value(void) const
	{ return _value_; }
};

struct Integer : public Real
{
	Integer(int value) :
		Real(value, MOT::MO_INTEGER)
	{}

	std::string to_string(void) const override
	{ return std::to_string((int)_value_); }

	int value(void) const
	{ return _value_; }
};

struct None : public MathObj
{
	None(void) :
		MathObj(MOT::MO_NONE)
	{}

	std::string to_string(void) const override
	{ return "none"; }
};

struct Variable : public MathObj
{
	std::string name;
	MathObjType _value_type_;
	std::shared_ptr<MathObj> value;

	Variable(std::string_view name, MathObjType type) :
		MathObj(MOT::MO_VARIABLE, type.is_const),
		name(name),
		_value_type_(type),
		value(std::shared_ptr<None>())
	{}
	Variable(std::string name) :
		Variable(name, MathObjType::MO_NONE)
	{}

	std::string to_string(void) const override
	{ return name; }

	MathObjType value_type(void) const
	{ return _value_type_; }
};

extern std::unordered_map<MOT, std::string> mathobjtype_string;

std::string mathobjtype_to_string(MOT type);
std::shared_ptr<MathObj> get_value(std::shared_ptr<MathObj> & obj);

#endif // MATHOBJ_H