#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "init.h"

class Algorithm
{
public:
    Algorithm();
    static Eigen::VectorXd solver(const Eigen::SparseMatrix <double> &A, const Eigen::VectorXd &b);
};

#endif // ALGORITHM_H
