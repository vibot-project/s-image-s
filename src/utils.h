#ifndef UTILS_H
#define UTILS_H

// Including global include section
#include "init.h"

class Utils
{
public:
    Utils();
    // Static value for the solver
    static Eigen::VectorXd solver(const Eigen::SparseMatrix <double> &A, const Eigen::VectorXd &b);
};

#endif // UTILS_H
