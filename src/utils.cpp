#include "utils.h"

Utils::Utils()
{
}

// Specifying the solver
Eigen::VectorXd Utils::solver(const Eigen::SparseMatrix <double> &A, const Eigen::VectorXd &b)
{
    // Ax=b solver. returns a vector x
    static Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > simplicialSolver;
    qDebug() << "Starting to solve Ax = b";
    simplicialSolver.compute(A);
    if(simplicialSolver.info()!=Eigen::Success){
        qDebug() << "Failed to compute A";
    }
    qDebug()<< "Solver computed A successfully";
    Eigen::VectorXd x = simplicialSolver.solve(b);
    if(simplicialSolver.info() != Eigen::Success){
        qDebug()<< "Failed to solve Ax = b";
    }

    qDebug() << "Ax = b solved!";
    return x;
}
