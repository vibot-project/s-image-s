/*
 * ImageLoad class is used for loading an image, fetching seeds
*/


#ifndef IMAGELOAD_H
#define IMAGELOAD_H

#include "init.h"

class ImageLoad
{
private:
    cv::Mat                 *image;                          //the image to be segmented
    std::set<int>           *fseeds;                   //foreground seeds coordinates
    std::set<int>           *bseeds;                   //background seeds coordinates
public:
    ImageLoad(const std::string &filename, const std::set<int> &fg, const std::set<int> &bg);
    cv::Mat                 getImage();
    std::set <int>          getFgSeeds();
    std::set <int>          getBgSeeds();
    ~ImageLoad();
};

#endif // IMAGELOAD_H
