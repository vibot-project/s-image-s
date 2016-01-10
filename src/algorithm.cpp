#include "algorithm.h"

// declaring constants for displacements along rows (dx) and columns (dy)
const int Algorithm::dx[] = {1, 0, -1, 0};
const int Algorithm::dy[] = {0, 1, 0, -1};
// declaring small number used for adding to weights between pixels
const double Algorithm::eps = 0.000001;

// Constructor for initializing image, seeds and tuning constants
Algorithm::Algorithm(const cv::Mat &image,
                               const std::set<std::pair<int,int> > &fgSeeds,
                               const std::set<std::pair<int,int> > &bgSeeds,
                               const double &_sigma,
                               const double &_beta,
                               const double &_Xb,
                               const double &_Xf):sigma(_sigma),beta(_beta),Xb(_Xb),Xf(_Xf)
{
    if(!image.empty() && !fgSeeds.empty() && !bgSeeds.empty())
        init(image, fgSeeds, bgSeeds);
}

// call functions for initializing sparse matrices and seeds
void Algorithm::init(const cv::Mat &image, const std::set<std::pair<int,int> > &fgSeeds, const std::set<std::pair<int,int> > &bgSeeds){
    initialImage = image.clone();
    rows = image.rows;
    cols = image.cols;
    declareSparse(rows, cols);
    initSeeds(fgSeeds, bgSeeds);
    initSparse(image);
}

// Call solver to solve the equation and generate new image
cv::Mat Algorithm::getSegmentation(){
    x = Utils::solver(A, b);
    qDebug() << "Applying new labels (colors)";
    qDebug() << Xb << Xf << endl;
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            if(x(i*cols+j) > (Xb+Xf)/2.){
                // set pixel to black if it belongs to background
                initialImage.at<cv::Vec3f>(i,j)[0] = 0;
                initialImage.at<cv::Vec3f>(i,j)[1] = 0;
                initialImage.at<cv::Vec3f>(i,j)[2] = 0;
            }
    return initialImage;
}

Algorithm::~Algorithm() {}

void Algorithm::declareSparse(int N, int M)
{
    // Pre-allocation of the matrix space and resize to the dimension
    Wij.resize(N*M, N*M);
    D.resize(N*M, N*M);
    Is.resize(N*M, N*M);
    L.resize(N * M, N * M);
    b.resize(N*M);

    // Reserving the values
    Wij.reserve(Eigen::VectorXf::Constant(N * M, 4));
    D.reserve(Eigen::VectorXf::Constant(N * M, 1));
    Is.reserve(Eigen::VectorXf::Constant(N * M, 1));
    L.reserve(Eigen::VectorXf::Constant(N * M, 5));
}

void Algorithm::initSeeds(const std::set<std::pair<int,int> > &fgSeeds, const std::set<std::pair<int,int> > &bgSeeds)
{
    // Getting the seeds and initialize them.
    for(std::set<std::pair<int,int> > :: iterator it = fgSeeds.begin(); it != fgSeeds.end(); it++)
        fseeds.insert((it->first)*cols+(it->second));
    for(std::set<std::pair<int,int> > :: iterator it = bgSeeds.begin(); it != bgSeeds.end(); it++)
        bseeds.insert((it->first)*cols+(it->second));
}

// genearte the sparse matrices.
void Algorithm::initSparse(const cv::Mat &image)
{
    qDebug() << "initializing sparse...";
    double w, sw;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            sw = 0.;
            for(int k = 0; k < 4; k++){
                // skip the iteration if it is in the border (out of boundary error prevention)
                if((i+dy[k] < 0 || i+dy[k] >= rows)||(j+dx[k] < 0 || j+dx[k] >= cols)) continue;
                // compute inf-norm between two pixels
                w = cv::norm(image.at<cv::Vec3f>(i, j), image.at<cv::Vec3f>(i+dy[k], j+dx[k]), cv::NORM_INF);
                // get weight
                w = exp(-(beta*w*w)/(sigma)) + eps;
                WijTriplet.push_back(Eigen::Triplet<double>(i*cols+j, (i+dy[k])*cols+j+dx[k], w));
                // accumulate sum of weights of neighbors of pixel (i,j)
                sw += w;
            }
            DTriplet.push_back(Eigen::Triplet<double>(i*cols+j, i*cols+j, sw));
            //if pixel at (i, j) belongs to foreground
            if(fseeds.count(i*cols+j) != 0){
                b(i*cols+j) = Xf;
                IsTriplet.push_back(Eigen::Triplet<double>(i*cols+j, i*cols+j, 1.));
            } else
            //if pixel at (i, j) belongs to background
            if(bseeds.count(i*cols+j) != 0){
                b(i*cols+j) = Xb;
                IsTriplet.push_back(Eigen::Triplet<double>(i*cols+j, i*cols+j, 1.));
            } else {
                b(i*cols+j) = 0;
            }
        }
    }
    // initialize Weight adjacency matrix from vectors of triplets
    Wij.setFromTriplets(WijTriplet.begin(), WijTriplet.end());
    // initialize diagonal matrix of valency weights
    D.setFromTriplets(DTriplet.begin(), DTriplet.end());
    // diagonal matrix of seeds (pixel in bseeds || pixel in fseeds):1 ? 0
    Is.setFromTriplets(IsTriplet.begin(), IsTriplet.end());
    L = D - Wij;
    A = Is + L * L;
    qDebug() << "initializing sparse finished";
}
