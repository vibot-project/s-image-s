#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollBar>
#include "init.h"
#include "imageload.h"
#include "workingcontext.h"
#include "algorithm.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QLabel *imageLabel;
    Ui::MainWindow *ui;
    ImageLoad *imload;
    WorkingContext *context;
    Algorithm *algo;

    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void showImage(const cv::Mat &image);
};

#endif // MAINWINDOW_H
