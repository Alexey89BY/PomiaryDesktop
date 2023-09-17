#include "formrange.h"
#include "ui_formrange.h"

FormRange::FormRange(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRange)
{
    ui->setupUi(this);
}


FormRange::~FormRange()
{
    delete ui;
}


void FormRange::setTitle(QString const& title)
{
    ui->labelTitle->setText(title);
}


void FormRange::setRange(PointRange const& range)
{
    ui->spinRangeZero->setValue(range.m_zero);
    ui->spinRangeError->setValue(range.m_error);
}
