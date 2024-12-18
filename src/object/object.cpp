#include "class/class.hpp"
#include "object/object.hpp"
#include "object/none_object.hpp"


ObjectPtr Object::none = nullptr;


void Object::init_properties(void)
{
	//TODO: Update this to add the superclasses' properties
	// Initialize the properties from the class
	for (auto & prop : m_class->properties())
	{
		properties.define(prop->name(), prop);
	}
}

void Object::set_property(std::string_view name, const ObjectPtr & value)
{
	auto prop = properties[name].second;
	prop->set(value);
}

bool Object::is_instance_of(ClassPtr cls) const
{
	return m_class->is_sub_class(cls);
}

mpfr_float add_reals(const mpfr_float & lhs, const mpfr_float & rhs)
{
	auto lhs_precision = mpfr_get_prec(lhs.backend().data());
	auto rhs_precision = mpfr_get_prec(rhs.backend().data());
	auto max_precision = std::max(lhs_precision, rhs_precision);

	mpfr_t result;
	mpfr_init2(result, max_precision);

	mpfr_add(result, lhs.backend().data(), rhs.backend().data(), MPFR_RNDN);
	mpfr_float result_mpfr(result); // Convert the result to an mpfr object
	mpfr_clear(result); // Clear the temporary result

	return result_mpfr;
}