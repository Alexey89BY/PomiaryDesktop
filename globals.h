#ifndef GLOBALS_H
#define GLOBALS_H

#include <QList>
#include <QString>
#include "pointrange.h"

class Globals
{
public:
    Globals() = delete;

    static QStringList titlesSeals;
    static QStringList filesSeals;
    static QStringList pointsP6Titles;
    static QStringList pointsP7Titles;
    static QList<PointRange> rangesMaxiPointsP6;
    static QList<PointRange> rangesMaxiPointsP7;
    static QList<PointRange> rangesStandardPointsP6;
    static QList<PointRange> rangesStandardPointsP7;

    static QStringList cuttingP6Titles;
    static QStringList cuttingP7Titles;
    static QList<PointRange> rangesMaxiCuttingP6;
    static QList<PointRange> rangesMaxiCuttingP7;
    static QList<PointRange> rangesStandardCuttingP6;
    static QList<PointRange> rangesStandardCuttingP7;

    static QStringList moldingsStandardP6Titles;
    static QStringList moldingsStandardP7Titles;
    static QStringList moldingsMaxiP6Titles;
    static QStringList moldingsMaxiP7Titles;
    static QList<PointRange> rangesMaxiMoldingsP7;
    static QList<PointRange> rangesMaxiMoldingsP6;
    static QList<PointRange> rangesStandardMoldingsP7;
    static QList<PointRange> rangesStandardMoldingsP6;

    static void init();
    static bool isIndexSealMaxi(int index);
};

#endif // GLOBALS_H
