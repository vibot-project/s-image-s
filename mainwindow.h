#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QMainWindow>
#include <QLabel>
#include <QScrollBar>
#include "init.h"
#include "workingcontext.h"
#include "workerthread.h"
#include "algorithm.h"
#include "labeldraw.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void                showImage(const QImage &image, const QString &message);

private slots:
    void                on_Button_open_clicked();
    void                on_horizontalSlider_valueChanged(int value);
    void                on_algstart_clicked();
    void                on_process_clicked();
    void                on_ground_clicked();
    void                mousePressed();
    void                mouseMoved();
    void                errorHandler(QString);

private:
    LabelDraw                           *imageLabel;
    Ui::MainWindow                      *ui;
    WorkerThread                        *worker;
    QThread                             *thread;
    int                                 xpointStart, xpointEnd;
    int                                 ypointStart, ypointEnd;
    QImage                              timage;
    QImage                              rimage;
    cv::Mat                             cvimage;
    std::set<std::pair<int,int> >       fseeds;
    std::set<std::pair<int,int> >       bseeds;
    QPen                                paintpen;
    bool                                buttonpro;
    bool                                select;
    bool                                paint;
    clock_t                             timeBegin;
    clock_t                             timeEnd;
    //void                                showImage(const cv::Mat &image);
    bool                                loadImg(const QString &);

};

#endif // MAINWINDOW_H
