#ifndef QGRIDCAMERAVIEWFINDER_H
#define QGRIDCAMERAVIEWFINDER_H

#include <QCameraViewfinder>

class QGridCameraViewfinder : public QCameraViewfinder
{
protected:
    void paintEvent(QPaintEvent *event) override;

public:
    QGridCameraViewfinder(QWidget *parent = nullptr);
    void setTargetSize(qreal size);

private:
    qreal targetWidth = 0;
};

#endif // QGRIDCAMERAVIEWFINDER_H
