#ifndef DIALOGHISTORY_H
#define DIALOGHISTORY_H

#include <QDialog>
#include <QTableWidget>
#include "pointdata.h"


namespace Ui {
class DialogHistory;
}

class DialogHistory : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHistory(QWidget *parent = nullptr);
    ~DialogHistory();

    static bool appendToHistory(int sealTypeId,
                                QString const& currentTime, QString const& currentOperator,
                                QList<PointData> const& pointsData);

private:
    Ui::DialogHistory *ui;

    void appendColumns(QTableWidget *tableWidget, QStringList const &headerTitles);
    void readHistory(int sealTypeId, QTableWidget *tableWidget);
};

#endif // DIALOGHISTORY_H
