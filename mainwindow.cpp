#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialoghistory.h"
#include "dialogsettings.h"
#include <QDateTime>
#include <QMessageBox>
#include <QCameraInfo>
#include "globals.h"
#include "settings.h"


double MainWindow::currentCoordX = 0.0;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("%1 - %2 %3").arg(QApplication::applicationName(), __DATE__, __TIME__));

    on_buttonSetZeroX_clicked();

    widgetsPointsP6 << ui->widgetPointP6_1 << ui->widgetPointP6_2 << ui->widgetPointP6_3 << ui->widgetPointP6_4 << ui->widgetPointP6_5
                    << ui->widgetPointP6_6 << ui->widgetPointP6_7 << ui->widgetPointP6_8 << ui->widgetPointP6_9 << ui->widgetPointP6_10;
    widgetsPointsP7 << ui->widgetPointP7_1 << ui->widgetPointP7_2 << ui->widgetPointP7_3;

    setPointsTitles(widgetsPointsP6, Globals::pointsP6Titles);
    setPointsTitles(widgetsPointsP7, Globals::pointsP7Titles);
    ui->comboNewSealType->addItems(Globals::titlesSeals);

    Settings::load();

    setCamera(QString());

    connect(&timerRefreshCoordX, &QTimer::timeout, this, &MainWindow::slot_timerRefreshCoordX_timeout);
    timerRefreshCoordX.start(500);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setCamera(QString const& deviceName)
{
    auto deviceNameLatin1 = (deviceName.isEmpty())
        ? QCameraInfo::defaultCamera().deviceName().toLatin1()
        : deviceName.toLatin1();
    m_camera.reset(new QCamera(deviceNameLatin1));

    m_camera->setViewfinder(ui->widgetViewFinder);
    m_camera->setCaptureMode(QCamera::CaptureViewfinder);
    m_camera->start();
}


void MainWindow::setPointsTitles(QList<FormPoint*> const &widgetsPoints, QStringList const& titles)
{
    int index = 0;
    foreach (auto &widgetPoint, widgetsPoints)
    {
        widgetPoint->setTitle(titles.at(index));
        ++index;
    }
}


void MainWindow::setPointsRanges(QList<FormPoint*> const &widgetsPoints, QList<PointRange> const &rangesPoints)
{
    int index = 0;
    foreach (auto &widgetPoint, widgetsPoints)
    {
        if (index < rangesPoints.size())
        {
            widgetPoint->setVisible(true);
            widgetPoint->setRange(rangesPoints.at(index));
        }
        else
        {
            widgetPoint->setVisible(false);
            widgetPoint->clear();
        }

        ++index;
    }
}


void MainWindow::clearPoints(QList<FormPoint*> const &widgetsPoints)
{
    foreach (auto &widgetPoint, widgetsPoints)
    {
        widgetPoint->setVisible(true);
        widgetPoint->clear();
    }
}


bool MainWindow::isPointsMeasured(QList<FormPoint*> const &widgetsPoints)
{
    foreach (auto &widgetPoint, widgetsPoints)
    {
        if (widgetPoint->isVisible() && widgetPoint->isCleared())
        {
            return (false);
        }
    }

    return (true);
}


void MainWindow::appendPointsToList(QList<PointData>& pointsData, QList<FormPoint*> const& widgetsPoints)
{
    foreach (auto &widgetPoint, widgetsPoints)
    {
        if (widgetPoint->isVisible())
        {
            pointsData.append(widgetPoint->getPointData());
        }
    }
}


double MainWindow::getCurrentCoordX()
{
    return currentCoordX;
}


void MainWindow::on_buttonSetZeroX_clicked()
{
    currentCoordX = 0.0;
    refreshCoordX();
}


void MainWindow::slot_timerRefreshCoordX_timeout()
{
    currentCoordX += rand() / 5000.0;
    refreshCoordX();
}


void MainWindow::refreshCoordX()
{
    ui->labelCurrentX->setText(QString::number(currentCoordX, 'f', 2));
}


void MainWindow::on_buttonNewMeasure_clicked()
{
    ui->frameCurrentMeasure->setEnabled(true);
    ui->frameNewMeasure->setEnabled(false);
    ui->labelCurrentSealType->setText(ui->comboNewSealType->currentText());
    ui->labelCurrentTime->setText(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (Globals::isIndexSealMaxi(ui->comboNewSealType->currentIndex()))
    {
        setPointsRanges(widgetsPointsP6, Globals::rangesMaxiPointsP6);
        setPointsRanges(widgetsPointsP7, Globals::rangesMaxiPointsP7);
    }
    else
    {
        setPointsRanges(widgetsPointsP6, Globals::rangesStandardPointsP6);
        setPointsRanges(widgetsPointsP7, Globals::rangesStandardPointsP7);
    }

    ui->framePointsP6->setEnabled(true);
    ui->framePointsP7->setEnabled(true);
}


void MainWindow::on_buttonSaveMeasure_clicked()
{
    if (! (isPointsMeasured(widgetsPointsP6) && isPointsMeasured(widgetsPointsP7)))
    {
        QMessageBox::critical(this, windowTitle(), tr("Not all points are measured!"));
        return;
    }

    if (QMessageBox::question(this, windowTitle(), tr("Do you want to SAVE measurements?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes)
    {
        return;
    }

    QList<PointData> pointsData;
    appendPointsToList(pointsData, widgetsPointsP6);
    appendPointsToList(pointsData, widgetsPointsP7);

    if (! DialogHistory::appendToHistory(ui->comboNewSealType->currentIndex(),
                                        ui->labelCurrentTime->text(), ui->labelCurrentOperator->text(), pointsData))
    {
        QMessageBox::critical(this, windowTitle(), tr("Unable to save measures!"));
        return;
    }

    ui->frameCurrentMeasure->setEnabled(false);
    ui->frameNewMeasure->setEnabled(true);
    ui->framePointsP6->setEnabled(false);
    ui->framePointsP7->setEnabled(false);
}


void MainWindow::on_buttonCancelMeasure_clicked()
{
    if (QMessageBox::warning(this, windowTitle(), tr("Do you want to CANCEL measurements?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes)
    {
        return;
    }

    ui->frameCurrentMeasure->setEnabled(false);
    ui->frameNewMeasure->setEnabled(true);
    ui->framePointsP6->setEnabled(false);
    ui->framePointsP7->setEnabled(false);
    ui->labelCurrentSealType->clear();
    ui->labelCurrentTime->clear();
    clearPoints(widgetsPointsP6);
    clearPoints(widgetsPointsP7);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (ui->frameNewMeasure->isEnabled())
    {
        event->accept();
        return;
    }

    on_buttonCancelMeasure_clicked();

    if (ui->frameNewMeasure->isEnabled())
    {
        event->accept();
        return;
    }

    event->ignore();
}


void MainWindow::on_buttonHistory_clicked()
{
    QScopedPointer<DialogHistory> dialog(new DialogHistory());
    dialog->exec();
}


void MainWindow::on_buttonSettings_clicked()
{
    QScopedPointer<DialogSettings> dialog(new DialogSettings());
    connect(dialog.data(), &QDialog::accepted, this, &MainWindow::slot_dialogSettings_accepted);
    dialog->exec();
}


void MainWindow::slot_dialogSettings_accepted()
{
    auto dialog = qobject_cast<DialogSettings*>(sender());
    Settings::save();

    setCamera(dialog->getCameraDeviceName());
}

