/*
 * ImageLoad class is used for loading an image, fetching seeds
*/


#ifndef IMAGELOAD_H
#define IMAGELOAD_H

#include "init.h"

class ImageLoad
{
private:
    cv::Mat image;                          //the image to be segmented
    cv::Mat fgSeeds;                        //foreground seeds
    cv::Mat bgSeeds;                        //background seeds
public:
    ImageLoad(std::string, int);
    cv::Mat getImage();
    ~ImageLoad();
};

#endif // IMAGELOAD_H
