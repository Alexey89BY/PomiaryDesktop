#include "framezoom.h"
#include <QResizeEvent>
//#include <QDebug>


FrameZoom::FrameZoom(QWidget *parent) :QFrame(parent)
{

}


void FrameZoom::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    zoomChild();
}


void FrameZoom::zoomChild()
{
    QWidget *child = dynamic_cast<QWidget *>(children().at(0));

    QSize widgetSize = child->sizeHint();
    QSize frameSize = size();

    //qDebug() << frameSize.width() << "x" << frameSize.height() << " @ " << widgetSize.width() << "x" << widgetSize.height();

    auto newHeight = frameSize.height();
    auto newWidth = frameSize.width();

    if ((frameSize.height() * widgetSize.width()) < (frameSize.width() * widgetSize.height()))
    {
        newHeight = (newWidth * widgetSize.height() ) / widgetSize.width();
    }
    else
    {
        newWidth = (newHeight * widgetSize.width() ) / widgetSize.height();
    }

    auto newX = (frameSize.width() - newWidth) / 2;
    auto newY = (frameSize.height() - newHeight) / 2;

    child->setGeometry(newX, newY, newWidth, newHeight);
}
