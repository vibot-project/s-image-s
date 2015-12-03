#include "labeldraw.h"

LabelDraw::LabelDraw(QWidget *parent):QLabel(parent)
{
}

void LabelDraw::mousePressEvent(QMouseEvent *event)
{
    xpoint = event->x();
    ypoint = event->y();
    emit mousePressed();
}

void LabelDraw::mouseMoveEvent(QMouseEvent *event)
{
    xpoint = event->x();
    ypoint = event->y();
    emit mouseMoved();
}

int LabelDraw::getX()
{
    return xpoint;
}

int LabelDraw::getY()
{
    return ypoint;
}
