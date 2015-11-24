#include "imageload.h"

ImageLoad::ImageLoad(std::string filepath, std::string filename, int flags = CV_LOAD_IMAGE_COLOR)
{
    this->image = cv::imread(filepath+filename, flags);
    this->fgSeeds = cv::imread(filepath+"f.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    this->bgSeeds = cv::imread(filepath+"b.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    if(this->image.empty() || this->fgSeeds.empty() || this->bgSeeds.empty()){
        qDebug() << "IMAGE_LOAD: Image is not loaded!";
    }
    qDebug() << image.type();
    image.convertTo(image, CV_32FC3);                 //Normalize image
    qDebug() << image.type();
}

cv::Mat ImageLoad::getImage()
{
    return this->image;
}

cv::Mat ImageLoad::getFgSeeds()
{
    return this->fgSeeds;
}

cv::Mat ImageLoad::getBgSeeds()
{
    return this->bgSeeds;
}

ImageLoad::~ImageLoad()
{
    this->image.release();
    this->fgSeeds.release();
    this->bgSeeds.release();
}
