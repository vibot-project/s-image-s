#ifndef ALGORITHM_H
#define ALGORITHM_H

// Including the classes
#include "init.h"
#include "utils.h"

class Algorithm
{
private:

    // Static delcaration
    static const int dx[];                                  //adjacent pixel indices. horizontal(x)-direction
    static const int dy[];                                  //adjacent pixel indices. vertical(y)-direction
    static const double eps;

    /**
     * Tunining constants.
     */
    double sigma;// = 0.1;                                     //initialization to constructor
    double beta;// = 0.00001;                                  //initialization to constructor
    double Xb;// = 3.;                                         //Background label
    double Xf;// = 1.;                                         //Foreground label

    int rows;                                                  //Number of rows of the image
    int cols;                                                  //Number of columns of the image
    Eigen::SparseMatrix <double> Wij;                       //weight adjacency matrix
    Eigen::SparseMatrix <double> D;                         //diagonal matrix of valency weights
    Eigen::SparseMatrix <double> Is;                        //diagonal matrix of seeds (pixel in bseeds || pixel in fseeds):1 ? 0
    Eigen::SparseMatrix <double> L;
    Eigen::SparseMatrix <double> A;
    std::vector <Eigen::Triplet <double> > WijTriplet;
    std::vector <Eigen::Triplet <double> > DTriplet;
    std::vector <Eigen::Triplet <double> > IsTriplet;
    Eigen::VectorXd b;                                             //b[i] = Xb if i in bseeds; elseif i in fseeds b[i] = Xf; else b[i] = 0;
    Eigen::VectorXd x;
    std::set <int> fseeds;
    std::set <int> bseeds;

    cv::Mat initialImage;

    // Functions for the matrix and seed initialization.
    void init(const cv::Mat &image, const std::set<std::pair<int,int> > &fgSeeds, const std::set<std::pair<int,int> > &bgSeeds);
    void declareSparse(int, int);
    void initSeeds(const std::set<std::pair<int,int> > &fgSeeds, const std::set<std::pair<int,int> > &bgSeeds);
    void initSparse(const cv::Mat &image);
public:
    // Various constructor for the algorithm
    Algorithm();
    Algorithm(const Algorithm& context);
    Algorithm(const cv::Mat &image,
                   const std::set<std::pair<int,int> > &fgSeeds,
                   const std::set<std::pair<int,int> > &bgSeeds,
                   const double &sigma=0.1,
                   const double &beta=0.001,
                   const double &Xb=3.,
                   const double &Xf=1.);
    int getRows();
    int getCols();
    cv::Mat getSegmentation();
    ~Algorithm();
};

#endif // Algorithm_H
