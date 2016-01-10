#-------------------------------------------------
#
# Project created by QtCreator 2015-11-06T20:56:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = seoop
TEMPLATE = app


unix {
    LIBS += `pkg-config opencv --libs`
}

mac {
    #LIBS += opencv libs
    #LIBS += eigen libs
INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib \
   -lopencv_core \
   -lopencv_highgui \
   -lopencv_imgproc \
   -lopencv_features2d \
   -lopencv_calib3d \
   -lopencv_nonfree \
   -lopencv_flann \
   -lopencv_legacy \
   -lopencv_stitching

}

win32 {
INCLUDEPATH += C:\opencv\opencv\mingw\install\include
LIBS += -L"C:/opencv/opencv/mingw/install/x64/mingw/bin"
LIBS += -lopencv_core2411 -lopencv_highgui2411 -lopencv_imgproc2411

INCLUDEPATH += C:\eigen
}

CONFIG += c++11

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/workerthread.cpp \
    src/labeldraw.cpp \
    src/utils.cpp \
    src/algorithm.cpp

HEADERS  += src/mainwindow.h \
    src/workerthread.h \
    src/init.h \
    src/labeldraw.h \
    src/utils.h \
    src/algorithm.h

FORMS    += src/mainwindow.ui
