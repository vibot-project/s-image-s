#ifndef LABELDRAW_H
#define LABELDRAW_H

// Initialize the include files
#include "init.h"
#include <QLabel>
#include <QMouseEvent>

class LabelDraw : public QLabel
{
    Q_OBJECT
public:
    // Defining for the labeldraw
    LabelDraw(QWidget *parent = 0);
    int getX();
    int getY();
private:
    // function for Mouse position tracking
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int xpoint;
    int ypoint;
signals:
    // Signals from MouseEvent
    void mousePressed();
    void mouseMoved();
};

#endif // LABELDRAW_H
