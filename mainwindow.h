#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
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

    bool buttonpro,select;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool loadImg(const QString &);

private slots:
    void on_Button_open_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_algstart_clicked();

    void on_process_clicked();

    void on_ground_clicked();

private:
    QLabel *imageLabel;
    Ui::MainWindow *ui;
    ImageLoad *imload;
    WorkingContext *context;
    Algorithm *algo;

    void showImage(const cv::Mat &image);
    void mouseMoveEvent( QMouseEvent* event );
};

#endif // MAINWINDOW_H
