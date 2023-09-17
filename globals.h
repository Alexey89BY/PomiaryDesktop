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

    static void init();
    static bool isIndexSealMaxi(int index);
};

#endif // GLOBALS_H
