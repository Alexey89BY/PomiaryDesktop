#ifndef WINDOWANALYSE_H
#define WINDOWANALYSE_H

#include <QMainWindow>

namespace Ui {
class WindowAnalyse;
}

class WindowAnalyse : public QMainWindow
{
    Q_OBJECT

public:
    explicit WindowAnalyse(QWidget *parent = nullptr);
    ~WindowAnalyse();

private slots:
    void on_buttonImport_clicked();

    void on_buttonPrint_clicked();

    void on_comboModel_activated(int index);

    void on_comboSide_activated(int index);

    void on_comboProcess_activated(int index);

    void on_comboProfile_activated(int index);

    void on_spinValuesLimit_valueChanged(int arg1);

    void on_comboTimestamp_activated(int index);

    void on_spinSigmaLevel_valueChanged(double arg1);

    void on_buttonAutoPrint_clicked();

    void on_checkDrawHistogram_stateChanged(int arg1);

    void on_spinHistogramColumns_valueChanged(int arg1);

    void on_checkDrawColoredRanges_stateChanged(int arg1);

private:
    Ui::WindowAnalyse *ui;

    QString defaultTitle;
    QList<QStringList> historyData;

    void filterData(QString const & pointsModel, QString const & pointsSide, QVector<QPair<int,int>> const & pointsMap,
                    QVector<QString> & pointsTimeStamps,
                    QVector<QVector<double>> & pointsVectors,
                    QVector<double> & pointsAverages,
                    QVector<double> & pointsStdev);
    void drawData(QPainter &painter, QRect const &paintRect, int indexModel, int indexSide, int indexProfile, int indexProcess);
    void refreshData();
    void autoPrint();

};

#endif // WINDOWANALYSE_H
