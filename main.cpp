#include "mainwindow.h"
#include "globals.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    Globals::init();

    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
