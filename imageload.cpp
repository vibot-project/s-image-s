#include "imageload.h"

ImageLoad::ImageLoad(std::string filename, const std::set<int> &fg, const std::set<int> &bg)
{
    image = cv::imread(filename);
    if(this->image.empty() || this->fgSeeds.empty() || this->bgSeeds.empty()){
        qDebug() << "IMAGE_LOAD: Image is not loaded!";
    }
    qDebug() << image.type();
    image.convertTo(image, CV_32FC3);                 //Normalize image
    qDebug() << image.type();
}

cv::Mat ImageLoad::getImage()
{
    return image;
}

std::set <int> ImageLoad::getFgSeeds()
{
    return fseeds;
}

std::set <int> ImageLoad::getBgSeeds()
{
    return bseeds;
}

ImageLoad::~ImageLoad()
{
    image->release();
    fseeds->clear();
    bseeds->clear();
    delete image;
    delete fseeds;
    delete bseeds;
}
