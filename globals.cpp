#include "globals.h"


QStringList Globals::titlesSeals;
QStringList Globals::filesSeals;
QStringList Globals::pointsP6Titles;
QStringList Globals::pointsP7Titles;
QList<PointRange> Globals::rangesMaxiPointsP6;
QList<PointRange> Globals::rangesMaxiPointsP7;
QList<PointRange> Globals::rangesStandardPointsP6;
QList<PointRange> Globals::rangesStandardPointsP7;


void Globals::init()
{
    titlesSeals << "Sill Seal Standard LH" << "Sill Seal Standard RH" << "Sill Seal Maxi LH" << "Sill Seal Maxi RH";
    filesSeals << "std_lh.csv" << "std_rh.csv" << "maxi_lh.csv" << "maxi_rh.csv";
    rangesMaxiPointsP6 << PointRange(14, 1.5) << PointRange(116, 2.5) << PointRange(218, 2.5) << PointRange(320, 2.5) << PointRange(422, 2.5)
                       << PointRange(524, 2.5) << PointRange(626, 2.5) << PointRange(728, 3) << PointRange(830, 2.5) << PointRange(842.5, 3);
    rangesMaxiPointsP7 << PointRange(22.5, 1.5) << PointRange(107.5, 2.5) << PointRange(130.5, 2.5);

    rangesStandardPointsP6 << PointRange(21, 1.5) << PointRange(123, 2.5) << PointRange(225, 2.5) << PointRange(327, 2.5) << PointRange(429, 2.5)
                           << PointRange(531, 2.5) << PointRange(633, 2.5) << PointRange(655, 3);
    rangesStandardPointsP7 << PointRange(29, 2.5) << PointRange(114, 2.5) << PointRange(149, 2.5);

    for (int index = 0; index < rangesMaxiPointsP6.size(); ++index)
    {
        pointsP6Titles.append(QString("P6 Profil (%1)").arg(index+1));
    }

    for (int index = 0; index < rangesMaxiPointsP7.size(); ++index)
    {
        pointsP7Titles.append(QString("P7 Profil (%1)").arg(index+1));
    }
}



bool Globals::isIndexSealMaxi(int index)
{
    switch (index)
    {
    case 2:
    case 3:
        return (true);
    default:
        return (false);
    }
}
