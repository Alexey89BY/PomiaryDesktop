#include "globals.h"


QStringList Globals::titlesSeals;
QStringList Globals::filesSeals;
QStringList Globals::pointsP6Titles;
QStringList Globals::pointsP7Titles;
QList<PointRange> Globals::rangesMaxiPointsP6;
QList<PointRange> Globals::rangesMaxiPointsP7;
QList<PointRange> Globals::rangesStandardPointsP6;
QList<PointRange> Globals::rangesStandardPointsP7;

QStringList Globals::cuttingP6Titles;
QStringList Globals::cuttingP7Titles;
QList<PointRange> Globals::rangesMaxiCuttingP6;
QList<PointRange> Globals::rangesMaxiCuttingP7;
QList<PointRange> Globals::rangesStandardCuttingP6;
QList<PointRange> Globals::rangesStandardCuttingP7;

QStringList Globals::moldingsStandardP6Titles;
QStringList Globals::moldingsStandardP7Titles;
QStringList Globals::moldingsMaxiP6Titles;
QStringList Globals::moldingsMaxiP7Titles;
QList<PointRange> Globals::rangesMaxiMoldingsP7;
QList<PointRange> Globals::rangesMaxiMoldingsP6;
QList<PointRange> Globals::rangesStandardMoldingsP7;
QList<PointRange> Globals::rangesStandardMoldingsP6;


void Globals::init()
{
    titlesSeals << "Sill Seal Standard LH" << "Sill Seal Standard RH" << "Sill Seal Maxi LH" << "Sill Seal Maxi RH";
    filesSeals << "std_lh.csv" << "std_rh.csv" << "maxi_lh.csv" << "maxi_rh.csv";

    // Sill seal
    rangesMaxiPointsP6 << PointRange(14, 1.5) << PointRange(116, 2.5) << PointRange(218, 2.5) << PointRange(320, 2.5) << PointRange(422, 2.5)
                       << PointRange(524, 2.5) << PointRange(626, 2.5) << PointRange(728, 3) << PointRange(830, 2.5) << PointRange(839.5, 3);

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

    // Cutting
    rangesMaxiCuttingP6 << PointRange(837, 2.0) << PointRange(14, 1.3) << PointRange(102, 1.0) << PointRange(102, 1.0) << PointRange(102, 1.0)
                       << PointRange(102, 1.0) << PointRange(102, 1.0) << PointRange(102, 1.0) << PointRange(102, 1.0) << PointRange(102, 1.0)
                       << PointRange(7, 1.0);

    rangesMaxiCuttingP7 << PointRange(123, 1.5) << PointRange(19.5, 1.0) << PointRange(84, 1.0) << PointRange(19.5, 1.0);

    rangesStandardCuttingP6 << PointRange(653, 2.0) << PointRange(21, 1.0) << PointRange(102, 1.0) << PointRange(102, 1.0) << PointRange(102, 1.0)
                           << PointRange(102, 1.0) << PointRange(102, 1.0) << PointRange(102, 1.0) << PointRange(20, 1.3);

    rangesStandardCuttingP7 << PointRange(142.5, 1.5) << PointRange(26, 1.0) << PointRange(84, 1.0) << PointRange(32.5, 1.0);

    cuttingP6Titles.append("P6 Length");
    for (int index = 0; index < rangesMaxiPointsP6.size(); ++index)
    {
        cuttingP6Titles.append(QString("P6 Cutting (%1)").arg(index+1));
    }

    cuttingP7Titles.append("P7 Length");
    for (int index = 0; index < rangesMaxiPointsP7.size(); ++index)
    {
        cuttingP7Titles.append(QString("P7 Cutting (%1)").arg(index+1));
    }

    // Moldings

    moldingsStandardP6Titles    << "P6 Profil (M6)"
                                << "Molding M6 (P6)";

    rangesStandardMoldingsP6    << PointRange(653.0, 2.5)
                                << PointRange(2.0, 1.0);

    moldingsStandardP7Titles    << "P7 Profil (M6)" << "P7 Profil (M7)"
                                << "Molding M6 (P7)" << "Molding M7 (P7)";

    rangesStandardMoldingsP7    << PointRange(3.0, 1.0) << PointRange(145.5, 2.5)
                                << PointRange(3.0, 1.0) << PointRange(3.5, 1.0);

    moldingsMaxiP6Titles    << "P6 Profil (M8)"
                            << "Molding M8 (P6)";

    rangesMaxiMoldingsP6    << PointRange(837.0, 2.5)
                            << PointRange(2.5, 1.0);

    moldingsMaxiP7Titles    << "P7 Profil (M8)" << "P7 Profil (M9)"
                            << "Molding M8 (P7)" << "Molding M9 (P7)";

    rangesMaxiMoldingsP7    << PointRange(3.0, 1.0) << PointRange(126.0, 2.5)
                            << PointRange(3.0, 1.0) << PointRange(4.5, 1.0);
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
