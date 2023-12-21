#ifndef DEBUG_H
#define DEBUG_H

#include <vector>
#include <memory>

#include "token.h"

void d_print_token(std::unique_ptr<Token> & token);

#endif // DEBUG_H