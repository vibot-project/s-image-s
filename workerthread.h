/*How to use qthreads in a proper way
 * https://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/
*/
#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "init.h"
#include "workingcontext.h"
#include <time.h>
#include <QImage>

class WorkerThread : public QObject
{
    Q_OBJECT
public:
    WorkerThread(const cv::Mat &image,
                 const std::set<std::pair<int,int> > &fgSeeds,
                 const std::set<std::pair<int,int> > &bgSeeds,
                 const double &sigma=0.1,
                 const double &beta=0.001,
                 const double &Xb=3.,
                 const double &Xf=1.);
    ~WorkerThread();
public slots:
    void process();
signals:
    void finished(QImage, QString);
    void error(QString err);
    void progressEvent(int, QString);
private:
    WorkingContext *context;
    cv::Mat resultImage;
    const cv::Mat image;
    const std::set<std::pair<int,int> > fgSeeds;
    const std::set<std::pair<int,int> > bgSeeds;
    const double sigma;
    const double beta;
    const double Xb;
    const double Xf;
    clock_t beginTime;
    clock_t endTime;
    QImage convertMat(const cv::Mat &pimage);
    void updateProgress(int);
};
#endif // WORKERTHREAD_H
