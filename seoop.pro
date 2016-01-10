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

SOURCES += src/apps/main.cpp\
        src/gui/mainwindow.cpp \
    src/common/workerthread.cpp \
    src/common/algorithm.cpp \
    src/common/workingcontext.cpp \
    src/gui/labeldraw.cpp \
    src/gui/help.cpp

HEADERS  += src/gui/mainwindow.h \
    src/common/workingcontext.h \
    src/common/workerthread.h \
    src/common/algorithm.h \
    src/common/init.h \
    src/gui/labeldraw.h \
    src/gui/help.h

FORMS    += src/gui/mainwindow.ui \
    src/gui/help.ui

INCLUDEPATH += src/gui \
               src/common \
               src/apps
