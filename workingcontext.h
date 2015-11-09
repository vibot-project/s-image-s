#ifndef WORKINGCONTEXT_H
#define WORKINGCONTEXT_H

#include "init.h"

class WorkingContext
{
private:
    const int dx[] = {1, 0, -1, 0};                         //adjacent pixel indecies. horizontal(x)-direction
    const int dy[] = {0, 1, 0, -1};                         //adjacent pixel indecies. vertical(y)-direction

    /**
     * Tunining constants.
     */
    double sigma = 0.1;                                     //TODO: add definition here
    double beta = 0.00001;                                  //TODO: add definition here
    double eps = 0.000001;                                  //TODO: add definition here
    double Xb = 3.;                                         //Background label
    double Xf = 1.;                                         //Foreground label

    int N;                                                  //Number of rows of the image
    int M;                                                  //Number of columns of the image
    Eigen::SparseMatrix <double> Wij;                       //weight adjacency matrix
    Eigen::SparseMatrix <double> D;                         //diagonal matrix of valency weights
    Eigen::SparseMatrix <double> Is;                        //diagonal matrix of seeds (pixel in bseeds || pixel in fseeds):1 ? 0
    VectorXd b;                                             //b[i] = Xb if i in bseeds; elseif i in fseeds b[i] = Xf; else b[i] = 0;
    vector <Triplet<double> > WijTriplet;
    vector <Triplet<double> > DTriplet;
    vector <Triplet<double> > IsTriplet;
    set <int> seeds;
    set <int> fseeds;
    set <int> bseeds;

    void declareSparse(int, int);
    void initSeeds(const cv::Mat &fgSeeds, const cv::Mat &bgSeeds);
    void initSparse(const cv::Mat &image);
public:
    WorkingContext(const cv::Mat &image, const cv::Mat &fgSeeds, const cv::Mat &bgSeeds);
};

#endif // WORKINGCONTEXT_H
