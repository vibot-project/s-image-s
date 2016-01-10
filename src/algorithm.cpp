#include "algorithm.h"

const int Algorithm::dx[] = {1, 0, -1, 0};
const int Algorithm::dy[] = {0, 1, 0, -1};
const double Algorithm::eps = 0.000001;

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

void Algorithm::init(const cv::Mat &image, const std::set<std::pair<int,int> > &fgSeeds, const std::set<std::pair<int,int> > &bgSeeds){
    initialImage = image.clone();
    rows = image.rows;
    cols = image.cols;
    declareSparse(rows, cols);
    initSeeds(fgSeeds, bgSeeds);
    initSparse(image);
}

cv::Mat Algorithm::getSegmentation(){
    x = Utils::solver(A, b);
    qDebug() << "Applying new labels (colors)";
    qDebug() << Xb << Xf << endl;
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            if(x(i*cols+j) > (Xb+Xf)/2.){
                initialImage.at<cv::Vec3f>(i,j)[0] = 0;
                initialImage.at<cv::Vec3f>(i,j)[1] = 0;
                initialImage.at<cv::Vec3f>(i,j)[2] = 0;
            }
    return initialImage;
}

Algorithm::~Algorithm()
{
    qDebug() << "working context destroyed.";
}

void Algorithm::declareSparse(int N, int M)
{
    Wij.resize(N*M, N*M);
    D.resize(N*M, N*M);
    Is.resize(N*M, N*M);
    L.resize(N * M, N * M);
    b.resize(N*M);

    Wij.reserve(Eigen::VectorXf::Constant(N * M, 4));
    D.reserve(Eigen::VectorXf::Constant(N * M, 1));
    Is.reserve(Eigen::VectorXf::Constant(N * M, 1));
    L.reserve(Eigen::VectorXf::Constant(N * M, 5));
}

void Algorithm::initSeeds(const std::set<std::pair<int,int> > &fgSeeds, const std::set<std::pair<int,int> > &bgSeeds)
{
    for(std::set<std::pair<int,int> > :: iterator it = fgSeeds.begin(); it != fgSeeds.end(); it++)
        fseeds.insert((it->first)*cols+(it->second));
    for(std::set<std::pair<int,int> > :: iterator it = bgSeeds.begin(); it != bgSeeds.end(); it++)
        bseeds.insert((it->first)*cols+(it->second));
}

void Algorithm::initSparse(const cv::Mat &image)
{
    qDebug() << "initializing sparse...";
    double w, sw;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            sw = 0.;
            for(int k = 0; k < 4; k++){
                if((i+dy[k] < 0 || i+dy[k] >= rows)||(j+dx[k] < 0 || j+dx[k] >= cols)) continue;
                w = cv::norm(image.at<cv::Vec3f>(i, j), image.at<cv::Vec3f>(i+dy[k], j+dx[k]), cv::NORM_INF);
                w = exp(-(beta*w*w)/(sigma)) + eps;
                //Wij.insert(i*cols+j, (i+dy[k])*cols+j+dx[k]) =  w;
                WijTriplet.push_back(Eigen::Triplet<double>(i*cols+j, (i+dy[k])*cols+j+dx[k], w));
                sw += w;
            }
            //D.insert(i*cols+j, i*cols+j) = sw;
            DTriplet.push_back(Eigen::Triplet<double>(i*cols+j, i*cols+j, sw));
            if(fseeds.count(i*cols+j) != 0){
                b(i*cols+j) = Xf;
                //Is.insert(i*cols+j, i*cols+j) = 1.;
                IsTriplet.push_back(Eigen::Triplet<double>(i*cols+j, i*cols+j, 1.));
            } else if(bseeds.count(i*cols+j) != 0){
                b(i*cols+j) = Xb;
                //Is.insert(i*cols+j, i*cols+j) = 1.;
                IsTriplet.push_back(Eigen::Triplet<double>(i*cols+j, i*cols+j, 1.));
            } else {
                b(i*cols+j) = 0;
            }
        }
    }
    Wij.setFromTriplets(WijTriplet.begin(), WijTriplet.end());
    D.setFromTriplets(DTriplet.begin(), DTriplet.end());
    Is.setFromTriplets(IsTriplet.begin(), IsTriplet.end());
    L = D - Wij;
    A = Is + L * L;
    qDebug() << "initializing sparse finished";
}
