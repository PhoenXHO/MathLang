#pragma once

#include <string>
#include <memory>
#include <functional>
#include <utility> // for `std::pair`
#include <sstream> // for `std::ostringstream`
#include <boost/multiprecision/gmp.hpp> // for `mpz_int`
#include <boost/multiprecision/mpfr.hpp> // for `mpfr_float`

#include "util/globals.hpp" // for `globals::error_handler`
#include "class/builtins.hpp" // for `ClassPtr`


using namespace boost::multiprecision;


class MathObj;
using MathObjPtr = std::shared_ptr<MathObj>;


class MathObj
{
protected:
	ClassPtr m_class;

public:
	static const MathObjPtr none;

	enum class Type
	{
		MO_NONE,
		MO_INTEGER,
		MO_REAL,
		
		MO_REFERENCE
	};

	MathObj(ClassPtr cls) : m_class(cls) {}
	MathObj() : m_class(Builtins::mathobj_class) {}
	virtual ~MathObj() = default;

	ClassPtr get_class(void) const { return m_class; }
	bool is_instance_of(ClassPtr cls) const;

	virtual Type type(void) const = 0;
	virtual std::string to_string(void) const = 0;

	virtual MathObjPtr add(const MathObjPtr & rhs) const = 0;

	std::ostream & operator<<(std::ostream & os) const
	{
		os << to_string();
		return os;
	}
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
}

mpfr_float add_reals(const mpfr_float & lhs, const mpfr_float & rhs);