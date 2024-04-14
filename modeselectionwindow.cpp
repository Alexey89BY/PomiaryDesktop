#include "modeselectionwindow.h"
#include "ui_modeselectionwindow.h"
#include <QScopedPointer>
#include "dialogsettings.h"
#include "dialoghistory.h"
#include "mainwindow.h"


static QScopedPointer<DialogHistory> dialogHistory;
static QScopedPointer<MainWindow> mainWindow;


ModeSelectionWindow::ModeSelectionWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ModeSelectionWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("%1 - %2 %3").arg(QApplication::applicationName(), __DATE__, __TIME__));

    mainWindow.reset(new MainWindow());
    dialogHistory.reset(new DialogHistory());
}

ModeSelectionWindow::~ModeSelectionWindow()
{
    delete ui;
}


void ModeSelectionWindow::on_buttonMeasures_clicked()
{
    mainWindow->show();
}


void ModeSelectionWindow::on_buttonSettings_clicked()
{
    QScopedPointer<DialogSettings> dialog(new DialogSettings());
    connect(dialog.data(), &QDialog::accepted, mainWindow.get(), &MainWindow::slot_dialogSettings_accepted);
    dialog->exec();
}


void ModeSelectionWindow::on_buttonHistory_clicked()
{
    dialogHistory->show();
}


void ModeSelectionWindow::on_buttonAnalyse_clicked()
{

}

