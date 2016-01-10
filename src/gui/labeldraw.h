#ifndef LABELDRAW_H
#define LABELDRAW_H

#include "init.h"
#include <QLabel>
#include <QMouseEvent>

class LabelDraw : public QLabel
{
    Q_OBJECT
public:
    LabelDraw(QWidget *parent = 0);
    int getX();
    int getY();
private:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int xpoint;
    int ypoint;
signals:
    void mousePressed();
    void mouseMoved();
};

#endif // LABELDRAW_H
