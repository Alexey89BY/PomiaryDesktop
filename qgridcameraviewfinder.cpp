#include "qgridcameraviewfinder.h"
#include <QPainter>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>


QGridCameraViewfinder::QGridCameraViewfinder(QWidget *parent)
    :QCameraViewfinder(parent)
{

}


void QGridCameraViewfinder::setTargetSize(qreal size)
{
    targetWidth = size;
}


void QGridCameraViewfinder::paintEvent(QPaintEvent *event)
{
    QCameraViewfinder::paintEvent(event);

    QPainter painter(this);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::white);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    auto size = this->videoSurface()->surfaceFormat().frameSize();

    painter.drawText(10,100, QString("%1 x %2").arg(size.width()).arg(size.height()));

    qreal scale = width() / (qreal)size.width();

    qreal centerX = width() * 0.5;
    qreal centerY = height() * 0.5;
    painter.drawEllipse(QPointF(centerX, centerY), targetWidth * scale, targetWidth * scale);
    painter.drawLine(QPointF(0, centerY), QPointF(width(), centerY));
    painter.drawLine(QPointF(centerX, 0), QPointF(centerX, height()));
}
