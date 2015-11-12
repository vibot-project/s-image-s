#include "algorithm.h"

Algorithm::Algorithm()
{

}

Eigen::VectorXd Algorithm::solver(const Eigen::SparseMatrix <double> &A, const Eigen::VectorXd &b)
{
    qDebug() << "Starting to solve Ax = b";
    solverA.compute(A);
    if(solverA.info()!=Eigen::Success){
        qDebug() << "Failed to compute A";
    }
    qDebug()<< "Solver computed A successfully";
    x = solverA.solve(b);
    if(solverA.info() != Eigen::Success){
        qDebug()<< "Failed to solve Ax = b";
    }

    qDebug() << "Ax = b solved!";
    return x;
}
