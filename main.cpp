#include "modeselectionwindow.h"
#include "globals.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    Globals::init();

    QApplication a(argc, argv);
    ModeSelectionWindow w;
    w.showNormal();
    return a.exec();
}
