#ifndef BUILTIN_H
#define BUILTIN_H

#include "mathobj.h"
#include "operator.h"

// Binary operators
extern OperatorFunction ml__add__real_real;
extern OperatorFunction ml__subtract__real_real;
extern OperatorFunction ml__multiply__real_real;
extern OperatorFunction ml__divide__real_real;
extern OperatorFunction ml__exponentiate__real_real;
extern OperatorFunction ml__assign__;

// Unary operators
extern OperatorFunction ml__negate__real;
extern OperatorFunction ml__print__real;

#endif // BUILTIN_H
