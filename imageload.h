/*
 * ImageLoad class is used for loading an image, fetching seeds
*/


#ifndef IMAGELOAD_H
#define IMAGELOAD_H

#include "init.h"

class ImageLoad
{
private:
    cv::Mat image;
    cv::Mat fgSeeds;
    cv::Mat bgSeeds;
public:
    ImageLoad();

};

#endif // IMAGELOAD_H
