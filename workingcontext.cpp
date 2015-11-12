#include "workingcontext.h"


//using WorkingContext::dx;
//using WorkingContext::dy;
const int WorkingContext::dx[] = {1, 0, -1, 0};
const int WorkingContext::dy[] = {0, 1, 0, -1};

WorkingContext::WorkingContext(const cv::Mat &image, const cv::Mat &fgSeeds, const cv::Mat &bgSeeds)//:dx({1, 0, -1, 0}),dy({0, 1, 0, -1})
{
    if(!image.empty() && !fgSeeds.empty() && !bgSeeds.empty())
    {
        N = image.rows;
        M = image.cols;
        declareSparse(N, M);
        initSeeds(fgSeeds, bgSeeds);
        initSparse(image);
    }
}

void WorkingContext::declareSparse(int N, int M)
{
    Wij.resize(N*M, N*M);
    Wij.reserve(N*M*4);
    D.resize(N*M, N*M);
    D.reserve(N*M);
    Is.resize(N*M, N*M);
    Is.reserve(N*M);
    b.resize(N*M);
}

void WorkingContext::initSeeds(const cv::Mat &fgSeeds, const cv::Mat &bgSeeds)
{
    for(int i = 0; i < N; i++)
        for(int j = 0; j < M; j++){
            int colorB = int(bgSeeds.at<uchar>(i, j));
            int colorF = int(fgSeeds.at<uchar>(i, j));
            if(colorB < 200){
                seeds.insert(i*M+j);
                bseeds.insert(i*M+j);
            }
            if(colorF < 200){
                seeds.insert(i*M+j);
                fseeds.insert(i*M+j);
            }
        }
}

void WorkingContext::initSparse(const cv::Mat &image)
{
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            double sw = 0.;
            for(int k = 0; k < 4; k++){
                if((i+dy[k] < 0 || i+dy[k] >= N)||(j+dx[k] < 0 || j+dx[k] >= M)) continue;
                double w;
                w = cv::norm(image.at<cv::Vec3f>(i, j), image.at<cv::Vec3f>(i+dy[k], j+dx[k]), cv::NORM_INF);
                w = exp(-(beta*w*w)/(sigma)) + eps;
                WijTriplet.push_back(Eigen::Triplet<double>(i*M+j, (i+dy[k])*M+j+dx[k], w));
                sw += w;
            }
            DTriplet.push_back(Eigen::Triplet<double>(i*M+j, i*M+j, sw));
            seeds.count(i*M+j) != 0 ? IsTriplet.push_back(Eigen::Triplet<double>(i*M+j, i*M+j, 1.)) : IsTriplet.push_back(Eigen::Triplet<double>(i*M+j, i*M+j, 0.));
            if(fseeds.count(i*M+j) != 0)
                b(i*M+j) = Xf;
            else if(bseeds.count(i*M+j) != 0)
                b(i*M+j) = Xb;
            else
                b(i*M+j) = 0;
        }
    }

    Wij.setFromTriplets(WijTriplet.begin(), WijTriplet.end());
    D.setFromTriplets(DTriplet.begin(), DTriplet.end());
    Is.setFromTriplets(IsTriplet.begin(), IsTriplet.end());
}

