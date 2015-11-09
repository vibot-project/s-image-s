#include "imageload.h"

ImageLoad::ImageLoad(std::string file, int flags = CV_LOAD_IMAGE_COLOR)
{
    this->image = cv::imread(file, flags);
    this->fgSeeds = cv::imread(file, CV_LOAD_IMAGE_GRAYSCALE);
    this->bgSeeds = cv::imread(file, CV_LOAD_IMAGE_GRAYSCALE);
    if(this->image.empty() || this->fgSeeds.empty() || this->bgSeeds.empty()){
        qDebug() << "Image is not loaded!";
    }
    image.convertTo(image, CV_32F);                 //Normalize image
}

ImageLoad::~ImageLoad()
{
    this->image.release();
    this->fgSeeds.release();
    this->bgSeeds.release();
}
