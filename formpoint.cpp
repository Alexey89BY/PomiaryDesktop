#include "formpoint.h"
#include "ui_formpoint.h"
#include "mainwindow.h"
#include <cmath>


const auto roundFactor = 0.1;
const auto errorEpsilon = roundFactor / 2;


FormPoint::FormPoint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPoint)
{
    ui->setupUi(this);
}


FormPoint::~FormPoint()
{
    delete ui;
}


void FormPoint::setTitle(QString const &title)
{
    ui->labelTitle->setText(title);
    ui->buttonSetCoord->setText(tr("Set"));
}


void FormPoint::setRange(PointRange const &range)
{
    clear();

    pointRange = range;
    ui->labelRange->setText(QString("%1 \u00B1 %2 mm\n%3 - %4 mm")
                                .arg(pointRange.m_zero, 0, 'f', 1).arg(pointRange.m_error, 0, 'f', 1)
                                .arg(pointRange.m_zero-pointRange.m_error, 0, 'f', 1).arg(pointRange.m_zero+pointRange.m_error, 0, 'f', 1));
}


void FormPoint::clear()
{
    pointRange = PointRange();
    pointData = PointData();
    ui->labelRange->clear();
    ui->labelCoord->clear();
    ui->labelResult->clear();
}


bool FormPoint::isCleared() const
{
    return (! pointData.m_isSetted);
}


PointData const& FormPoint::getPointData() const
{
    return (pointData);
}


static bool isValidPoint(PointRange const& range, double value)
{
    return ((std::abs(value - range.m_zero) - range.m_error) < errorEpsilon);
}


void FormPoint::on_buttonSetCoord_clicked()
{
    auto coordX = std::abs(MainWindow::getCurrentCoordX());

    pointData.m_value = round(coordX / roundFactor) * roundFactor;
    pointData.m_isValid = isValidPoint(pointRange, pointData.m_value);
    pointData.m_isSetted = true;

    ui->labelCoord->setText(QString::number(pointData.m_value, 'f', 1));

    QString resultText = QString("<html><head/><body><p><span style='color:%1;'>%2</span></p></body></html>").arg(
            (pointData.m_isValid)? "#00ff00": "#ff0000",
            (pointData.m_isValid)? tr("OK"): tr("NOK")
        );
    ui->labelResult->setText(resultText);
}

