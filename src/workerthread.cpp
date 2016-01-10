#include "workerthread.h"

// constructor for the workerthread
WorkerThread::WorkerThread(const cv::Mat &image,
                           const std::set<std::pair<int,int> > &fgSeeds,
                           const std::set<std::pair<int,int> > &bgSeeds,
                           const double &sigma,
                           const double &beta,
                           const double &Xb,
                           const double &Xf):image(image),fgSeeds(fgSeeds),bgSeeds(bgSeeds),sigma(sigma),beta(beta),Xb(Xb),Xf(Xf)
{
    context = NULL;
}

// Initializing the start of the algorithm and updating the time and statusbar.
void WorkerThread::process()
{
    // starting the clock
    beginTime = clock();
    updateProgress(10);
    // creating the dynamic member for the algorithm
    context = new Algorithm(image, fgSeeds, bgSeeds, sigma, beta, Xb, Xf);
    updateProgress(20);
    resultImage = context->getSegmentation();
    updateProgress(90);
    //Collecting the segmented image
    QImage qimage;
    qimage = convertMat(resultImage);
    endTime = clock();
    // Calculating the process timing
    float processTime = (float(endTime) - float(beginTime)) / CLOCKS_PER_SEC;
    QString message = QString("Processed in %1 seconds.").arg(processTime);
    updateProgress(100);
    // let to thread know that job is done and send segmented image
    emit finished(qimage, message);
}

// Converting the opencv image to qimage
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

// Update of the progress bar
void WorkerThread::updateProgress(int value)
{
    emit progressEvent(value, QString("Process"));
}

WorkerThread::~WorkerThread()
{
    delete context;
    qDebug() << "worker thread destroyed";
}
