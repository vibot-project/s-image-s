#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Including all nessary class.
#include <QtWidgets>
#include <QMainWindow>
#include <QLabel>
#include <QScrollBar>
#include "init.h"
#include "workingcontext.h"
#include "workerthread.h"
#include "algorithm.h"
#include "labeldraw.h"
#include "help.h"

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
    void                showImage(const QImage &image, const QString &message); // showing the image
    void                progressUpdate(int, QString); // Updating the process

private slots:
    // UI generated classes
    void                on_Button_open_clicked();
    void                on_horizontalSlider_valueChanged(int value);
    void                on_algstart_clicked();
    void                on_process_clicked();
    void                on_ground_currentIndexChanged(int index);
    void                on_uiFgrNum_currentIndexChanged(int index);
    void                on_uiMaxWidth_editingFinished();
    void                on_uiMaxHeight_editingFinished();
    void                on_saveButton_clicked();
    void                on_actionQuit_triggered();
    void                on_actionAbout_triggered();
    void                on_actionHelp_triggered();

    // Mouse action.
    void                mousePressed();
    void                mouseMoved();
    void                errorHandler(QString);


private:
    // Initializing the variables
    int                                 xpointStart, xpointEnd;
    int                                 ypointStart, ypointEnd;
    bool                                buttonpro;
    int                                 select;
    bool                                paint;

    // creating the member for the class
    LabelDraw                           *imageLabel;
    Ui::MainWindow                      *ui;
    WorkerThread                        *worker;
    QThread                             *thread;
    QProgressBar                        *progressBar;
    Help                                ui_help;
    // Initializing the values for different actions
    QImage                              timage;
    QImage                              rimage;
    cv::Mat                             cvimage;
    std::set<std::pair<int,int> >       fseeds;
    std::set<std::pair<int,int> >       bseeds;
    QPen                                paintpen;
    // Values for the time check
    clock_t                             timeBegin;
    clock_t                             timeEnd;
    // Load image function
    bool                                loadImg(const QString &);

};

#endif // MAINWINDOW_H
