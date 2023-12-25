#include "dialogsettings.h"
#include "globals.h"
#include "ui_dialogsettings.h"
#include <QCameraInfo>
#include <QtSerialPort/QSerialPortInfo>


static void setupRanges(QList<FormRange*> widgetsRanges, QStringList const& pointsTitles, QList<PointRange> const& pointsRanges);


DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);

    ui->comboCamera->addItem(tr("*** default camera ***"), QString());
    auto listCameraInfos = QCameraInfo::availableCameras();
    for (auto&& cameraInfo: listCameraInfos)
    {
        ui->comboCamera->addItem(cameraInfo.description(), cameraInfo.deviceName());
    }

    ui->comboSerialPort->addItem(tr("*** disconnected ***"));
    auto listSerialPorts = QSerialPortInfo::availablePorts();
    for (auto&& portInfo: listSerialPorts)
    {
        ui->comboSerialPort->addItem(portInfo.description(), portInfo.portName());
    }

    widgetsRangesStandardP6 << ui->widgetRangeStandardP6_1 << ui->widgetRangeStandardP6_2 << ui->widgetRangeStandardP6_3
                            << ui->widgetRangeStandardP6_4 << ui->widgetRangeStandardP6_5 << ui->widgetRangeStandardP6_6
                            << ui->widgetRangeStandardP6_7 << ui->widgetRangeStandardP6_8;
    widgetsRangesStandardP7 << ui->widgetRangeStandardP7_1 << ui->widgetRangeStandardP7_2 << ui->widgetRangeStandardP7_3;

    setupRanges(widgetsRangesStandardP6, Globals::pointsP6Titles, Globals::rangesStandardPointsP6);
    setupRanges(widgetsRangesStandardP7, Globals::pointsP7Titles, Globals::rangesStandardPointsP7);

    widgetsRangesMaxiP6 << ui->widgetRangeMaxiP6_1 << ui->widgetRangeMaxiP6_2 << ui->widgetRangeMaxiP6_3
                        << ui->widgetRangeMaxiP6_4 << ui->widgetRangeMaxiP6_5 << ui->widgetRangeMaxiP6_6
                        << ui->widgetRangeMaxiP6_7 << ui->widgetRangeMaxiP6_8 << ui->widgetRangeMaxiP6_9
                        << ui->widgetRangeMaxiP6_10;
    widgetsRangesMaxiP7 << ui->widgetRangeMaxiP7_1 << ui->widgetRangeMaxiP7_2 << ui->widgetRangeMaxiP7_3;

    setupRanges(widgetsRangesMaxiP6, Globals::pointsP6Titles, Globals::rangesMaxiPointsP6);
    setupRanges(widgetsRangesMaxiP7, Globals::pointsP7Titles, Globals::rangesMaxiPointsP7);
}


static void setupRanges(QList<FormRange*> widgetsRanges, QStringList const& pointsTitles, QList<PointRange> const& pointsRanges)
{
    int index = 0;
    for (auto&& form : widgetsRanges)
    {
        form->setTitle(pointsTitles.at(index));
        form->setRange(pointsRanges.at(index));
        ++index;
    }
}



DialogSettings::~DialogSettings()
{
    delete ui;
}


QString DialogSettings::getCameraDeviceName() const
{
    return (ui->comboCamera->currentData().toString());
}


QString DialogSettings::getSerialPortName() const
{
    return (ui->comboSerialPort->currentData().toString());
}

