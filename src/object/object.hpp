#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include <memory>
#include <functional>
#include <utility> // for `std::pair`
#include <sstream> // for `std::ostringstream`
#include <boost/multiprecision/gmp.hpp> // for `mpz_int`
#include <boost/multiprecision/mpfr.hpp> // for `mpfr_float`

#include "global/globals.hpp"

//using mpz = boost::multiprecision::mpz_int; // Arbitrary precision integer
//using mpfr = boost::multiprecision::mpfr_float; // Arbitrary precision floating point
using namespace boost::multiprecision;

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
		MO_REAL,
		
		MO_REFERENCE
	};

	MathObj() = default;
	virtual ~MathObj() = default;

	virtual Type type(void) const = 0;
	virtual std::string to_string(void) const = 0;

	virtual MathObjPtr add(const MathObjPtr & rhs) const = 0;

	std::ostream & operator<<(std::ostream & os) const
	{
		os << to_string();
		return os;
	}
};

class IntegerObj : public MathObj
{
	mpz_int value;
	size_t size;

public:
	IntegerObj(const mpz_int & value) : value(value), size(value.str().size()) {}
	~IntegerObj() = default;

	mpz_int get_value(void) const { return value; }
	size_t get_size(void) const { return size; }

	Type type(void) const override { return Type::MO_INTEGER; }
	std::string to_string(void) const override { return value.str(); }

	MathObjPtr add(const MathObjPtr & rhs) const override;
};

class RealObj : public MathObj
{
	mpfr_float value;
	size_t integer_part;
	size_t decimal_part;

public:
	RealObj(std::string_view value) : value(value)
	{
		size_t dot_pos = value.find('.');
		integer_part = dot_pos;
		decimal_part = value.size() - dot_pos - 1;
	}
	RealObj(const mpfr_float & value, size_t integer_part, size_t decimal_part)
		: value(value), integer_part(integer_part), decimal_part(decimal_part)
	{}
	~RealObj() = default;

	mpfr_float get_value(void) const { return value; }
	size_t get_integer_part(void) const { return integer_part; }
	size_t get_decimal_part(void) const { return decimal_part; }

	Type type(void) const override { return Type::MO_REAL; }
	std::string to_string(void) const override
	{
		std::ostringstream oss;
		oss << std::setprecision(integer_part + decimal_part) << value;
		return oss.str();

		//char * str = new char[integer_part + decimal_part + 2];
		//mpfr_sprintf(str, "%.*Rf", integer_part + decimal_part, value.backend().data());
		//std::string result(str);
		//delete[] str;
		//return result;
	}

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

mpfr_float add_reals(const mpfr_float & lhs, const mpfr_float & rhs);

#endif // OBJECT_HPP