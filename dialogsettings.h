#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include "formrange.h"
#include <QDialog>

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = nullptr);
    ~DialogSettings();

    QString getCameraDeviceName() const;

private:
    Ui::DialogSettings *ui;
    QList<FormRange*> widgetsRangesStandardP6;
    QList<FormRange*> widgetsRangesStandardP7;
    QList<FormRange*> widgetsRangesMaxiP6;
    QList<FormRange*> widgetsRangesMaxiP7;
};

#endif // DIALOGSETTINGS_H
