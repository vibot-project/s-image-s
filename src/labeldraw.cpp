#include "labeldraw.h"

LabelDraw::LabelDraw(QWidget *parent):QLabel(parent)
{
}

// Reading the mouse point and sending to the mouse press function
void LabelDraw::mousePressEvent(QMouseEvent *event)
{
    xpoint = event->x();
    ypoint = event->y();
    emit mousePressed();
}

// Read the position when the mouse is on the move
void LabelDraw::mouseMoveEvent(QMouseEvent *event)
{
    xpoint = event->x();
    ypoint = event->y();
    emit mouseMoved();
}
// Get the X and Y position of the mouse in label
int LabelDraw::getX()
{
    return xpoint;
}

int LabelDraw::getY()
{
    return ypoint;
}
