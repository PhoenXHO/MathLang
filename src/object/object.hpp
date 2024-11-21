#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <memory>
#include <functional>
#include <utility> // for `std::pair`

#include "global/globals.hpp"

//TODO: Make all objects instances of a base class (e.g. IntegerObj from Integer, RealObj from Real, etc.)
//TODO: Make all classes inherit from MathObj

class MathObj;
using MathObjPtr = std::shared_ptr<MathObj>;

class MathObj
{
public:
	enum class Type
	{
		MO_NONE,
		MO_INTEGER,
		MO_REAL
	};

	MathObj() = default;
	virtual ~MathObj() = default;

	virtual Type type(void) const = 0;
	virtual std::string to_string(void) const = 0;

	virtual MathObjPtr add(const MathObjPtr & rhs) const = 0;
};

class IntegerObj : public MathObj
{
	int value;

public:
	IntegerObj(int value) : value(value) {}
	~IntegerObj() = default;

	int get_value(void) const { return value; }

	Type type(void) const override { return Type::MO_INTEGER; }
	std::string to_string(void) const override { return std::to_string(value); }

	MathObjPtr add(const MathObjPtr & rhs) const override;
};

class RealObj : public MathObj
{
	double value;

public:
	RealObj(double value) : value(value) {}
	~RealObj() = default;

	double get_value(void) const { return value; }

	Type type(void) const override { return Type::MO_REAL; }
	std::string to_string(void) const override { return std::to_string(value); }

	MathObjPtr add(const MathObjPtr & rhs) const override;
};


// Hash specialization for std::pair<MathObj::Type, MathObj::Type>
namespace std
{
	template <>
	struct hash<std::pair<MathObj::Type, MathObj::Type>>
	{
		std::size_t operator()(const std::pair<MathObj::Type, MathObj::Type> & pair) const noexcept
		{
			std::size_t h1 = std::hash<MathObj::Type>{}(pair.first);
			std::size_t h2 = std::hash<MathObj::Type>{}(pair.second);
			return h1 ^ (h2 << 1);
		}
	};
};

#endif // OBJECT_HPP