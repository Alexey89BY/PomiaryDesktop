#ifndef FRAMEZOOM_H
#define FRAMEZOOM_H

#include <QFrame>


class FrameZoom : public QFrame
{
public:
    FrameZoom(QWidget *parent);

    void zoomChild();

private:
    void resizeEvent(QResizeEvent* event) override;
};

#endif // FRAMEZOOM_H
