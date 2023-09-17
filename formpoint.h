#ifndef FORMPOINT_H
#define FORMPOINT_H

#include <QWidget>
#include "pointrange.h"
#include "pointdata.h"


namespace Ui {
class FormPoint;
}

class FormPoint : public QWidget
{
    Q_OBJECT

public:
    explicit FormPoint(QWidget *parent = nullptr);
    ~FormPoint();

    void clear();
    void setTitle(QString const &title);
    void setRange(PointRange const &range);
    bool isCleared() const;
    PointData const& getPointData() const;

private slots:
    void on_buttonSetCoord_clicked();

private:
    Ui::FormPoint *ui;
    PointData pointData;
    PointRange pointRange;
};

#endif // FORMPOINT_H
