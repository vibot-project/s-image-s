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
    updateProgress(10);
    context = new WorkingContext(image, fgSeeds, bgSeeds, sigma, beta, Xb, Xf);
    updateProgress(20);
    resultImage = context->getSegmentation();
    updateProgress(90);
    QImage qimage;
    qimage = convertMat(resultImage);
    endTime = clock();
    float processTime = (float(endTime) - float(beginTime)) / CLOCKS_PER_SEC;
    QString message = QString("Processed in %1 seconds.").arg(processTime);
    updateProgress(100);
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

void WorkerThread::updateProgress(int value)
{
    emit progressEvent(value, QString("Process"));
}

WorkerThread::~WorkerThread()
{
    delete context;
}
