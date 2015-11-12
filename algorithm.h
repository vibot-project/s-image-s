#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "init.h"
#include "workingcontext.h"

class Algorithm
{
private:
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > solverA;
    Eigen::VectorXd x;
public:
    Algorithm();
    Eigen::VectorXd solver(const Eigen::SparseMatrix <double> &A, const Eigen::VectorXd &b);
};

#endif // ALGORITHM_H
