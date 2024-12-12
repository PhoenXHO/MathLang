#pragma once

#include "class.hpp"

struct ReferenceClass : public Class
{
	ReferenceClass(void) : Class("Reference") { init(); }

	void init(void);

	std::string to_string(void) const override
	{
		std::ostringstream oss;
		oss << "<Reference " << '(' << m_value_class->name() << ")>";
		return oss.str();
	}

private:
	ClassPtr m_value_class;
};