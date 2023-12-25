#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QList>
#include <QCamera>
#include <QCloseEvent>
#include <QScopedPointer>
#include <QSerialPort>
#include "formpoint.h"
#include "pointrange.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event) override;

    static double getCurrentCoordX();

private slots:
    void on_buttonHistory_clicked();
    void on_buttonSetZeroX_clicked();
    void on_buttonNewMeasure_clicked();
    void on_buttonSaveMeasure_clicked();
    void on_buttonCancelMeasure_clicked();
    void on_buttonSettings_clicked();
    void slot_dialogSettings_accepted();
    void slot_timerRefreshCoordX_timeout();
    void slot_serialPort_readyRead();

private:
    Ui::MainWindow *ui;
    QTimer timerRefreshCoordX;
    QList<FormPoint*> widgetsPointsP6;
    QList<FormPoint*> widgetsPointsP7;
    QScopedPointer<QCamera> m_camera;
    QScopedPointer<QSerialPort> m_serialport;

    void setCamera(QString const& deviceName);
    void setSerialPort(QString const& portName);
    void refreshCoordX();
    void pollSerialPort();
    void setPointsTitles(QList<FormPoint*> const &widgetsPoints, QStringList const& titles);
    void setPointsRanges(QList<FormPoint*> const &widgetsPoints, QList<PointRange> const &rangesPoints);
    void clearPoints(QList<FormPoint*> const &widgetsPoints);
    bool isPointsMeasured(QList<FormPoint*> const &widgetsPoints);
    void appendPointsToList(QList<PointData>& pointsData, QList<FormPoint*> const& widgetsPoints);

    static double currentCoordX;
};
#endif // MAINWINDOW_H
