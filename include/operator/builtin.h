#ifndef BUILTIN_H
#define BUILTIN_H

#include "mathobj.h"
#include "operator.h"

// Binary operators
extern BuiltinOpFunc ml__add__real_real;
extern BuiltinOpFunc ml__subtract__real_real;
extern BuiltinOpFunc ml__multiply__real_real;
extern BuiltinOpFunc ml__divide__real_real;
extern BuiltinOpFunc ml__exponentiate__real_real;
extern BuiltinOpFunc ml__assign__;

// Unary operators
extern BuiltinOpFunc ml__negate__real;
extern BuiltinOpFunc ml__print__real;
extern BuiltinOpFunc ml__print__none;

#endif // BUILTIN_H
