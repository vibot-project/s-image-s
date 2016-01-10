#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Creating a member abd showing for the application to start.
    MainWindow w;
    w.show();

    return a.exec();
}
