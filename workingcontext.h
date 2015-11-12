#ifndef WORKINGCONTEXT_H
#define WORKINGCONTEXT_H

#include "init.h"

class WorkingContext
{
private:
    static const int dx[];                                  //adjacent pixel indices. horizontal(x)-direction
    static const int dy[];                                  //adjacent pixel indices. vertical(y)-direction
    static const double eps;

    /**
     * Tunining constants.
     */
    double sigma;// = 0.1;                                     //TODO: add definition here & move initialization to constructor
    double beta;// = 0.00001;                                  //TODO: add definition here & move initialization to constructor
    double Xb;// = 3.;                                         //Background label
    double Xf;// = 1.;                                         //Foreground label

    int N;                                                  //Number of rows of the image
    int M;                                                  //Number of columns of the image
    Eigen::SparseMatrix <double> Wij;                       //weight adjacency matrix
    Eigen::SparseMatrix <double> D;                         //diagonal matrix of valency weights
    Eigen::SparseMatrix <double> Is;                        //diagonal matrix of seeds (pixel in bseeds || pixel in fseeds):1 ? 0
    Eigen::SparseMatrix <double> L;
    Eigen::SparseMatrix <double> A;
    Eigen::VectorXd b;                                             //b[i] = Xb if i in bseeds; elseif i in fseeds b[i] = Xf; else b[i] = 0;
    std::vector <Eigen::Triplet<double> > WijTriplet;
    std::vector <Eigen::Triplet<double> > DTriplet;
    std::vector <Eigen::Triplet<double> > IsTriplet;
    std::set <int> seeds;
    std::set <int> fseeds;
    std::set <int> bseeds;

    void declareSparse(int, int);
    void initSeeds(const cv::Mat &fgSeeds, const cv::Mat &bgSeeds);
    void initSparse(const cv::Mat &image);
public:
    WorkingContext(const cv::Mat &image,
                   const cv::Mat &fgSeeds,
                   const cv::Mat &bgSeeds,
                   const double &sigma=0.1,
                   const double &beta=0.001,
                   const double &Xb=3.,
                   const double &Xf=1.);
    Eigen::SparseMatrix <double> getA();
    Eigen::VectorXd getB();
    int getRows();
    int getCols();
};

#endif // WORKINGCONTEXT_H
