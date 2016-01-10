#ifndef UTILS_H
#define UTILS_H

#include "init.h"

class Utils
{
public:
    Utils();
    static Eigen::VectorXd solver(const Eigen::SparseMatrix <double> &A, const Eigen::VectorXd &b);
};

#endif // UTILS_H
