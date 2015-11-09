#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "init.h"
#include "workingcontext.h"

class Algorithm
{
public:
    Algorithm();
    static void solver(const WorkingContext &context);
};

#endif // ALGORITHM_H
