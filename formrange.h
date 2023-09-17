#ifndef FORMRANGE_H
#define FORMRANGE_H

#include "pointrange.h"
#include <QWidget>

namespace Ui {
class FormRange;
}

class FormRange : public QWidget
{
    Q_OBJECT

public:
    explicit FormRange(QWidget *parent = nullptr);
    ~FormRange();

    void setTitle(QString const& title);
    void setRange(PointRange const& range);

private:
    Ui::FormRange *ui;
};

#endif // FORMRANGE_H
