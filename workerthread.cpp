#include "workerthread.h"

WorkerThread::WorkerThread(const cv::Mat &image,
                           const std::set<std::pair<int,int> > &fgSeeds,
                           const std::set<std::pair<int,int> > &bgSeeds,
                           const double &sigma,
                           const double &beta,
                           const double &Xb,
                           const double &Xf):image(image),fgSeeds(fgSeeds),bgSeeds(bgSeeds),sigma(sigma),beta(beta),Xb(Xb),Xf(Xf)
{
}

void WorkerThread::process()
{
    beginTime = clock();
    context = new WorkingContext(image, fgSeeds, bgSeeds, sigma, beta, Xb, Xf);
    resultImage = context->getSegmentation();
    QImage qimage;
    qimage = convertMat(resultImage);
    endTime = clock();
    float processTime = (float(endTime) - float(beginTime)) / CLOCKS_PER_SEC;
    QString message = QString("Processed in %1 seconds.").arg(processTime);
    emit finished(qimage, message);
}

QImage WorkerThread::convertMat(const cv::Mat &pimage)
{
    cv::Mat image = pimage.clone();
    image.convertTo(image, CV_8U);
    QImage qimage;
    if( image.channels() == 3)
        qimage = QImage((const unsigned char*)(image.data),
                              image.cols, image.rows,
                              image.step, QImage::Format_RGB888).rgbSwapped();
    else if( image.channels() == 1)
        qimage = QImage((const unsigned char*)(image.data),
                              image.cols, image.rows,
                              image.step, QImage::Format_Indexed8);
    else
    {
        qDebug()<<"Error: wrong image format";
    }
    return qimage;
}

WorkerThread::~WorkerThread()
{
}
