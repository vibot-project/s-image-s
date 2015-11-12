#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    Ui::MainWindow *ui;
    ImageLoad *imload;
    WorkingContext *context;
    Algorithm *algo;
};

#endif // MAINWINDOW_H
