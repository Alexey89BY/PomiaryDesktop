#include "windowanalyse.h"
#include "ui_windowanalyse.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>
#include <QPainter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QPainterPath>
#include <QtMath>
#include "globals.h"


WindowAnalyse::WindowAnalyse(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WindowAnalyse)
{
    ui->setupUi(this);

    defaultTitle = windowTitle();
    ui->spinValuesLimit->setValue(ui->spinValuesLimit->maximum());
}


WindowAnalyse::~WindowAnalyse()
{
    delete ui;
}


void WindowAnalyse::on_buttonImport_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this);
    if (fileName.isEmpty())
        return;

    historyData.clear();
    ui->comboTimestamp->clear();

    QFile file(fileName);
    if (! file.open(QFile::ReadOnly | QFile::ExistingOnly))
    {
        auto message = QString("%1\n%2").arg(file.errorString(), fileName);
        QMessageBox::critical(this, defaultTitle, message);
    }

    if (file.isReadable())
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            const auto maximalLineLength = 512;
            const auto minimalLineLength = 16;

            auto historyLine = in.readLine(maximalLineLength);
            if (historyLine.length() >= maximalLineLength)
                break;

            if (historyLine.length() < minimalLineLength)
                continue;

            const auto fieldsDelimiter = '\t';
            const auto minimalTokensNumber = 3;

            auto fieldsList = historyLine.split(fieldsDelimiter);
            if (fieldsList.count() < minimalTokensNumber)
                continue;

            // decode timesatamp
            const auto timeStampLength = 14;

            auto timeStamp = fieldsList.at(0);
            if (timeStamp.length() != timeStampLength)
                continue;

            //QDateTime time = QDateTime::fromString(filedsList.at(0), "dd-MM-yy HH:mm");

            // Standard/Maxi

            // LH/RH

            // points p6

            // points p7

            // empty

            // raw points p6 (+cross)

            // raw points p7 (+2 crosses)

            //
            historyData.append(fieldsList);
        }
    }

    file.close();

    foreach (auto && fieldsList, historyData) {
        ui->comboTimestamp->addItem(fieldsList.at(0));
    }

    ui->labelTotalCount->setText( QString("%1 records").arg(historyData.count()));

    setWindowTitle(QString("%1 [%2]").arg(defaultTitle, fileName));

    refreshData();
}


static double decodeValue(QStringList const &textValues, int index)
{
    auto textValue = textValues.value(index, QString());

    if (textValue.isEmpty())
        return 0.0;

    if (textValue.contains('.'))
        return textValue.toDouble();

    return textValue.toInt() * 0.01;
}


void WindowAnalyse::filterData(QString const & pointsModel, QString const & pointsSide, QVector<QPair<int,int>> const & pointsMap,
                               QVector<QString> & pointsTimeStamps,
                               QVector<QVector<double>> & pointsVectors,
                               QVector<POINT_STAT> & pointsStats)
{
    QList<int> filterMap;
    if (ui->comboTimestamp->currentIndex() >= 0)
    {
        auto mapLimit = ui->spinValuesLimit->value();
        for (int index = ui->comboTimestamp->currentIndex(); index < historyData.count(); ++index)
        {
            auto const & data = historyData.at(index);
            if (
                (data.at(1) == pointsModel)
             && (data.at(2) == pointsSide)
               )
            {
                filterMap.append(index);
                if (filterMap.count() >= mapLimit)
                    break;
            }
        }
    }

    // timestamps
    pointsTimeStamps.resize(filterMap.count());
    for (int index = 0; index < filterMap.count(); ++index)
    {
        int dataIndex = filterMap.at(index);
        pointsTimeStamps[index] = historyData.at(dataIndex).at(0);
    }

    // values
    auto valuesCount = pointsMap.count();

    pointsVectors.resize(valuesCount);
    pointsStats.resize(valuesCount);

    for (int colIndex = 0; colIndex < valuesCount; ++colIndex)
    {
        auto values = QVector<double>(filterMap.count());

        for (int rowIndex = 0; rowIndex < filterMap.count(); ++rowIndex)
        {
            int dataIndex = filterMap.at(rowIndex);

            auto const &data = historyData.at(dataIndex);

            auto shiftedValue = decodeValue(data, pointsMap.at(colIndex).first);
            auto baseValue = decodeValue(data, pointsMap.at(colIndex).second);

            values[rowIndex] = shiftedValue - baseValue;
        }

        // average & stdev
        auto averageValue = 0.0;
        auto stdevValue = 0.0;

        if (values.count() > 0)
        {
            auto averageSum = 0.0;
            foreach (auto const & it, values)
            {
                averageSum += it;
            }

            averageValue = averageSum / values.count();
        }

        if (values.count() > 1)
        {
            auto stdevSum = 0.0;
            foreach (auto const & it, values)
            {
                stdevSum += (it - averageValue) * (it - averageValue);
            }

            auto sigmaScale = ui->spinSigmaLevel->value();
            stdevValue = sigmaScale * qSqrt(stdevSum / (values.count() - 1));
        }

        pointsStats[colIndex].average = averageValue;
        pointsStats[colIndex].stdev = stdevValue;
        pointsVectors[colIndex] = values;
    }
}


static void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags, const QString & text)
{
    const qreal size = 32767.0;
    QPointF corner(x, y - size);

    if (flags & Qt::AlignHCenter)
        corner.rx() -= size/2.0;
    else
    if (flags & Qt::AlignRight)
        corner.rx() -= size;

    if (flags & Qt::AlignVCenter)
        corner.ry() += size/2.0;
    else
        if (flags & Qt::AlignTop) corner.ry() += size;
    else
        flags |= Qt::AlignBottom;

    QRectF rect{corner.x(), corner.y(), size, size};
    painter.drawText(rect, flags, text);
}


static void drawText(QPainter & painter, const QPointF & point, Qt::Alignment flags, const QString & text)
{
    drawText(painter, point.x(), point.y(), flags, text);
}


static constexpr auto drawRangePadding = 2.0;
static constexpr double errorEpsilon = 0.01 / 2.;
static constexpr double errorWarning = 0.5;


static void drawRange(QPainter &painter, qreal x0, qreal y0, qreal scale, PointRange const &range, bool withMarks = false)
{
    const auto dxw = (range.m_error + drawRangePadding) * scale;
    const auto xpl = x0 - dxw;
    const auto xpr = x0 + dxw;

    painter.drawLine(QPointF(xpl, y0), QPointF(xpr, y0));

    const auto dyt = (withMarks)? 8.0: 4.0;

    painter.drawLine(QPointF(x0, y0), QPointF(x0, y0 + dyt));

    const auto xre = range.m_error * scale;

    painter.drawLine(QPointF(x0 + xre, y0), QPointF(x0 + xre, y0 + dyt));
    painter.drawLine(QPointF(x0 - xre, y0), QPointF(x0 - xre, y0 + dyt));

    if (withMarks)
    {
        const auto dyu = 5.0;

        auto xu = scale;
        while (xu < dxw)
        {
            painter.drawLine(QPointF(x0 + xu, y0), QPointF(x0 + xu, y0 + dyu));
            painter.drawLine(QPointF(x0 - xu, y0), QPointF(x0 - xu, y0 + dyu));
            xu += scale;
        }
    }
}


static void drawRangeText(QPainter &painter, qreal x0, qreal y0, qreal scale, PointRange const &range)
{
    auto xre = std::max(range.m_error * scale, 17.5);
    const auto dyt = 10;

    drawText(painter, QPointF(x0 - xre, y0 + dyt), Qt::AlignTop | Qt::AlignHCenter, QString::number(range.m_zero - range.m_error, 'f', 1));
    drawText(painter, QPointF(x0 + xre, y0 + dyt), Qt::AlignTop | Qt::AlignHCenter, QString::number(range.m_zero + range.m_error, 'f', 1));
}


static QColor getColorForPoint(double value, PointRange const &range)
{
    auto errorValue = std::abs(value - range.m_zero) - range.m_error;

    if (errorValue < -errorWarning)
    {
        return QColorConstants::Green;
    }

    if (errorValue < errorEpsilon)
    {
        return QColorConstants::Yellow;
    }


    return QColorConstants::Red;
}


static void drawMarker(QPainter &painter, qreal x0, qreal y0, qreal scale, PointRange const &range, double value, bool isFilled = true)
{
    auto xp = x0 + (value - range.m_zero) * scale;
    auto yp = y0;
    auto dpy = 10.0;
    auto dpx = 5.0;

    const auto dxw = (range.m_error + drawRangePadding) * scale;
    auto xpl = x0 - dxw;
    auto xpr = x0 + dxw;

    QPainterPath path;

    if (xp < xpl)
    {
        path.moveTo(xpl, yp - dpy);
        path.lineTo(xpl, yp);
        path.lineTo(xpl + dpx, yp - dpy);
    }
    else
    if (xp > xpr)
    {
        path.moveTo(xpr - dpx, yp - dpy);
        path.lineTo(xpr, yp);
        path.lineTo(xpr, yp - dpy);
    }
    else
    {
        path.moveTo(xp - dpx, yp - dpy);
        path.lineTo(xp, yp);
        path.lineTo(xp + dpx, yp - dpy);
    }

    path.closeSubpath();

    painter.drawPath(path);

    if (isFilled)
    {
        //auto pattern = (range.isValidPoint(value))? Qt::Dense4Pattern: Qt::SolidPattern;
        auto brush = QBrush(getColorForPoint(value, range));
        painter.fillPath(path, brush);
    }
}


static void drawRangeRect(QPainter &painter, qreal x0, qreal y0, qreal scale, PointRange const &range, int heightBottom)
{
    auto xle = x0 - scale * range.m_error;
    auto xre = x0 + scale * range.m_error;
    auto xll = xle - scale * drawRangePadding;
    auto xrr = xre + scale * drawRangePadding;
    auto xlw = xle + scale * errorWarning;
    auto xrw = xre - scale * errorWarning;

    painter.fillRect(QRect(QPoint(xll, y0), QPoint(xle, heightBottom)), QColor(255, 223, 223));
    painter.fillRect(QRect(QPoint(xre, y0), QPoint(xrr, heightBottom)), QColor(255, 223, 223));

    painter.fillRect(QRect(QPoint(xle, y0), QPoint(xlw, heightBottom)), QColor(255, 255, 223));
    painter.fillRect(QRect(QPoint(xrw, y0), QPoint(xre, heightBottom)), QColor(255, 255, 223));
}


void WindowAnalyse::drawData(QPainter &painter, QRect const &paintRect, int indexModel, int indexSide, int indexProfile, int indexProcess)
{
    painter.fillRect(paintRect, QBrush(QColorConstants::White));

    // Select data
    QStringList const *titles = nullptr;
    QList<PointRange> const *ranges = nullptr;
    QVector<QPair<int, int>> const *valuesMap;

    static QVector<QPair<int, int>> const mapSillSealStandardP7 {{29,27},{30,27},{32,27}};
    static QVector<QPair<int, int>> const mapSillSealStandardP6 {{18,17},{19,17},{20,17},{21,17},{22,17},{23,17},{24,17},{26,17}};

    static QVector<QPair<int, int>> const mapSillSealMaxiP7 {{33,31},{34,31},{36,31}};
    static QVector<QPair<int, int>> const mapSillSealMaxiP6 {{20,19},{21,19},{22,19},{23,19},{24,19},{25,19},{26,19},{27,19},{28,19},{30,19}};

    static QVector<QPair<int, int>> const mapCuttingStandardP7 {{31,28},{29,28},{30,29},{31,30}};
    static QVector<QPair<int, int>> const mapCuttingStandardP6 {{25,17},{18,17},{19,18},{20,19},{21,20},{22,21},{23,22},{24,23},{25,24}};

    static QVector<QPair<int, int>> const mapCuttingMaxiP7 {{35,32},{33,32},{34,33},{35,34}};
    static QVector<QPair<int, int>> const mapCuttingMaxiP6 {{29,19},{20,19},{21,20},{22,21},{23,22},{24,23},{25,24},{26,25},{27,26},{28,27},{29,28}};

    static QVector<QPair<int, int>> const mapMoldingsStandardP7 {{28,27},{32,31}};
    static QVector<QPair<int, int>> const mapMoldingsStandardP6 {{26,25}};

    static QVector<QPair<int, int>> const mapMoldingsMaxiP7 {{32,31},{36,35}};
    static QVector<QPair<int, int>> const mapMoldingsMaxiP6 {{30,29}};

    QString pointsModel = ui->comboModel->itemText(indexModel);
    QString pointsSide = ui->comboSide->itemText(indexSide);
    QString pointsProfile = ui->comboProfile->itemText(indexProfile);
    QString pointsProcess = ui->comboProcess->itemText(indexProcess);

    switch (indexModel)
    {
    case 1: // Maxi
        switch (indexProfile)
        {
        case 1: // P7
            switch (indexProcess)
            {
            case 1: // Cutting
                titles = &Globals::cuttingP7Titles;
                ranges = &Globals::rangesMaxiCuttingP7;
                valuesMap = &mapCuttingMaxiP7;
                break;
            case 2: // Moldings
                titles = &Globals::moldingsMaxiP7Titles;
                ranges = &Globals::rangesMaxiMoldingsP7;
                valuesMap = &mapMoldingsMaxiP7;
                break;
            default: // Sill seal
                titles = &Globals::pointsP7Titles;
                ranges = &Globals::rangesMaxiPointsP7;
                valuesMap = &mapSillSealMaxiP7;
                break;
            }
            break;

        default: // P6
            switch (indexProcess)
            {
            case 1: // Cutting
                titles = &Globals::cuttingP6Titles;
                ranges = &Globals::rangesMaxiCuttingP6;
                valuesMap = &mapCuttingMaxiP6;
                break;
            case 2: // Moldings
                titles = &Globals::moldingsMaxiP6Titles;
                ranges = &Globals::rangesMaxiMoldingsP6;
                valuesMap = &mapMoldingsMaxiP6;
                break;
            default: // Sill seal
                titles = &Globals::pointsP6Titles;
                ranges = &Globals::rangesMaxiPointsP6;
                valuesMap = &mapSillSealMaxiP6;
                break;
            }
            break;
        }
        break;

    default: // Standard
        switch (indexProfile)
        {
        case 1: // P7
            switch (indexProcess)
            {
            case 1: // Cutting
                titles = &Globals::cuttingP7Titles;
                ranges = &Globals::rangesStandardCuttingP7;
                valuesMap = &mapCuttingStandardP7;
                break;
            case 2: // Moldings
                titles = &Globals::moldingsStandardP7Titles;
                ranges = &Globals::rangesStandardMoldingsP7;
                valuesMap = &mapMoldingsStandardP7;
                break;
            default: // Sill seal
                titles = &Globals::pointsP7Titles;
                ranges = &Globals::rangesStandardPointsP7;
                valuesMap = &mapSillSealStandardP7;
                break;
            }
            break;

        default: // P6
            switch (indexProcess)
            {
            case 1: // Cutting
                titles = &Globals::cuttingP6Titles;
                ranges = &Globals::rangesStandardCuttingP6;
                valuesMap = &mapCuttingStandardP6;
                break;
            case 2: // Moldings
                titles = &Globals::moldingsStandardP6Titles;
                ranges = &Globals::rangesStandardMoldingsP6;
                valuesMap = &mapMoldingsStandardP6;
                break;
            default: // Sill seal
                titles = &Globals::pointsP6Titles;
                ranges = &Globals::rangesStandardPointsP6;
                valuesMap = &mapSillSealStandardP6;
                break;
            }
            break;
        }
        break;
    }

    QVector<QString> pointsTimeStamps;
    QVector<QVector<double>> pointsVectors;
    QVector<POINT_STAT> pointsStats;

    filterData(pointsModel, pointsSide, *valuesMap, pointsTimeStamps, pointsVectors, pointsStats);

    const auto tableTop = paintRect.top() + 5.0;
    const auto tableLeft = paintRect.left() + 5.0;
    const auto tableRight = paintRect.right() - 5.0;
    const auto tableBottom = paintRect.bottom() - 5.0;

    // Draw

    if (ui->checkDrawHistogram->isChecked())
    {
        // Draw histograms

        // table header

        const auto sigmaScale = ui->spinSigmaLevel->value();
        const auto sigmaPercent = (1.0 - std::erf(sigmaScale / std::sqrt(2.0))) * 100.0;

        drawText(painter, QPoint(tableLeft, tableTop), Qt::AlignTop | Qt::AlignLeft,
                 QString("%1 %2 %3 %4: %5 - %6\n\n%7\u03c3 %8%9")
                     .arg(pointsModel, pointsSide, pointsProfile, pointsProcess,
                        pointsTimeStamps.isEmpty()? "": pointsTimeStamps.at(0),
                        pointsTimeStamps.isEmpty()? "": pointsTimeStamps.at(pointsTimeStamps.count()-1)
                        )
                     .arg(sigmaScale, 0, 'f', 2).arg(sigmaPercent, 0, 'f', 2).arg('%'));

        const auto cellsTop = tableTop + 45;

        const auto cellsInWidth = 4;
        const auto cellsInHeight = 3;
        const auto cellWidth = (tableRight - tableLeft) / cellsInWidth;
        const auto cellHeight = (tableBottom - cellsTop) / cellsInHeight;

        for (auto rangeIndex = 0; rangeIndex < ranges->count(); ++rangeIndex)
        {
            const auto x = tableLeft + (rangeIndex % cellsInWidth) * cellWidth;
            const auto y = cellsTop + (rangeIndex / cellsInWidth) * cellHeight;

            auto x0 = x + cellWidth * 0.5;
            auto const & range = ranges->at(rangeIndex);
            auto const & pointsAverage = pointsStats.at(rangeIndex).average;
            auto const & pointsStdev = pointsStats.at(rangeIndex).stdev;

            drawText(painter, QPoint(x0, y), Qt::AlignTop | Qt::AlignHCenter, QString("%1\n%2 \u00b1 %3\n%4 \u00b1 %5")
                                                                                  .arg(titles->at(rangeIndex))
                                                                                  .arg(range.m_zero, 0, 'f', 1).arg(range.m_error, 0, 'f', 1)
                                                                                  .arg(pointsAverage, 0, 'f', 1).arg(pointsStdev, 0, 'f', 1));

            const int colsRangeNumber = ui->spinHistogramColumns->value();

            struct COLUMN
            {
                int counter;
                double leftX;
                double rightX;
            };

            QVector<COLUMN> columns(colsRangeNumber + 2);

            for (auto index = 0; index < columns.size(); ++index)
            {
                columns[index].counter = 0;

                const double colWidth = (2.0 * pointsStdev) / colsRangeNumber;

                columns[index].leftX = pointsAverage - pointsStdev + (index - 1) * colWidth;
                columns[index].rightX = columns[index].leftX + colWidth;
            }

            auto points = pointsVectors.at(rangeIndex);

            foreach (auto && pointValue, points) {
                int index = 0;
                while (
                    (index < columns.count() - 1)
                 && (pointValue > columns.at(index + 1).leftX)
                      )
                {
                    ++index;
                }

                ++(columns[index].counter);
            }

            int maxCount = 1;
            foreach (auto && col, columns) {
                if (col.counter > maxCount)
                {
                    maxCount = col.counter;
                }
            }

            const auto histPadding = 10.0;
            const auto histWidth = cellWidth - histPadding * 2.0;

            const auto histX0 = x + cellWidth / 2.0;

            const auto scale = ((histWidth * colsRangeNumber) / columns.count()) / (2.0 * pointsStdev);

            const auto yc = y + cellHeight - 40.0;
            const auto xm = (cellHeight - 115.0) / maxCount;

            if (ui->checkDrawColoredRanges->isChecked())
            {
                drawRangeRect(painter, histX0, y, scale, range, yc);
            }

            for (int colIndex = 0; colIndex < columns.count(); ++colIndex)
            {
                auto yt = yc - columns[colIndex].counter * xm;

                auto rect = QRect(
                    QPoint(histX0 + (columns[colIndex].leftX - pointsAverage) * scale, yc),
                    QPoint(histX0 + (columns[colIndex].rightX - pointsAverage) * scale - 1, yt)
                );

                auto value = (columns[colIndex].leftX + columns[colIndex].rightX) * 0.5;

                drawText(painter, QPoint((rect.left() + rect.right()) / 2, yt - 5), Qt::AlignBottom | Qt::AlignHCenter, QString("%1\n%2%3")
                                                                                                        .arg(columns[colIndex].counter)
                                                                                                        .arg((100.0*columns[colIndex].counter)/points.count(), 0, 'f', 1)
                                                                                                        .arg('%'));

                auto brush = QBrush(getColorForPoint(value, range));

                painter.fillRect(rect, brush);
                painter.drawRect(rect);
            }

            const auto stepsMm = ((histWidth * 0.5) / scale);
            const int subStep = 10;
            const int leftMm = std::ceil((pointsAverage - stepsMm) * subStep);
            const int rightMm = std::floor((pointsAverage + stepsMm) * subStep);

            for (int xmm = leftMm; xmm <= rightMm; ++xmm)
            {
                int xm = histX0 + (xmm / (double)subStep - pointsAverage) * scale;

                if (xmm % subStep == 0)
                {
                    const int dyc = 10;
                    painter.drawLine(xm, yc, xm, yc+dyc);
                    drawText(painter, QPoint(xm, yc+dyc), Qt::AlignTop | Qt::AlignHCenter, QString("%1").arg(xmm / subStep));
                }
                else
                {
                    const int dyc = 5;
                    painter.drawLine(xm, yc, xm, yc+dyc);
                }
            }

        }
    }
    else
    {
        // Draw graphs

        // table header

        const auto sigmaScale = ui->spinSigmaLevel->value();
        const auto sigmaPercent = (1.0 - std::erf(sigmaScale / std::sqrt(2.0))) * 100.0;

        drawText(painter, QPoint(tableLeft, tableTop), Qt::AlignTop | Qt::AlignLeft,
                 QString("%1 %2\n%3\n%4\n\n%5\u03c3 %6%7")
                     .arg(pointsModel, pointsSide, pointsProfile, pointsProcess)
                     .arg(sigmaScale, 0, 'f', 2).arg(sigmaPercent, 0, 'f', 2).arg('%'));

        const auto rowsTop = tableTop + 70.0;
        const auto colsLeft = tableLeft + 90.0;

        const auto colWidth = std::min(400.0, (tableRight - colsLeft) / ranges->count());
        const auto rowHeight = (tableBottom - rowsTop) / ui->spinValuesLimit->maximum();

        // table vertical header
        for (auto index = 0; index < pointsTimeStamps.count(); ++index)
        {
            auto x = tableLeft;
            auto y = rowsTop + index * rowHeight;

            auto const &data = pointsTimeStamps.at(index);
            drawText(painter, QPointF(x, y + rowHeight * 0.5), Qt::AlignVCenter | Qt::AlignLeft, QString("%1. %2").arg(index+1, 2, 10, QChar('0')).arg(data));
        }

        // table colunmns
        for (auto colIndex = 0; colIndex < ranges->count(); ++colIndex)
        {
            // column header

            auto x = colsLeft + colIndex * colWidth;
            auto y = tableTop;

            auto const & range = ranges->at(colIndex);

            auto rangeWidth = colWidth - 30;
            auto scale = rangeWidth  / (2.5 * (range.m_error + drawRangePadding));
            auto x0 = x + rangeWidth * 0.5;
            auto y0 = y + (rowsTop - tableTop) * 0.6;

            if (ui->checkDrawColoredRanges->isChecked())
            {
                drawRangeRect(painter, x0, rowsTop, scale, range, tableBottom);
            }

            drawText(painter, QPoint(x0, y), Qt::AlignTop | Qt::AlignHCenter, QString("%1\n%2 \u00b1 %3").arg(titles->at(colIndex)).arg(range.m_zero, 0, 'f', 1).arg(range.m_error, 0, 'f', 1));

            // draw header range
            drawRange(painter, x0, y0, scale, range, true);
            drawRangeText(painter, x0, y0, scale, range);

            auto values = pointsVectors.at(colIndex);
            auto averageValue = pointsStats.at(colIndex).average;
            auto stdevValue = pointsStats.at(colIndex).stdev;

            if (values.count())
            {
                drawMarker(painter, x0, y0, scale, range, averageValue, false);
                drawMarker(painter, x0, y0, scale, range, averageValue - stdevValue);
                drawMarker(painter, x0, y0, scale, range, averageValue + stdevValue);
            }

            // draw row header
            auto xv = x + colWidth;

            drawText(painter, QPoint(xv, y0), Qt::AlignVCenter | Qt::AlignRight, QString("%1\n\u00B1 %2").arg(averageValue, 0, 'f', 1).arg(stdevValue, 0, 'f', 1));

            // draw rows
            for (auto rowIndex = 0; rowIndex < values.count(); ++rowIndex)
            {
                y = rowsTop + rowIndex * rowHeight;

                auto pointValue = values.at(rowIndex);
                auto yp = y + rowHeight * 0.5;

                drawRange(painter, x0, yp, scale, range, false);

                drawMarker(painter, x0, yp, scale, range, pointValue);
                drawText(painter, QPoint(xv, yp), Qt::AlignVCenter | Qt::AlignRight, QString::number(pointValue, 'f', 2));
            }
        }
    }
}


void WindowAnalyse::refreshData()
{
    // Create canvas
    QPixmap pixmap(1754, 1240); // paper A4

    QPainter painter(&pixmap);

/*
    QMessageBox::information(this, windowTitle(), QString("font: %1; pixmap: %2, %3; rect: %4, %5, %6, %7, %8, %9.")
                                                      .arg(painter.font().pointSizeF())
                                                      .arg(pixmap.width())
                                                      .arg(pixmap.height())
                             .arg(pixmap.rect().left())
                             .arg(pixmap.rect().top())
                             .arg(pixmap.rect().right())
                             .arg(pixmap.rect().bottom())
                             .arg(pixmap.rect().width())
                             .arg(pixmap.rect().height()));
*/

    drawData(painter, pixmap.rect(), ui->comboModel->currentIndex(), ui->comboSide->currentIndex(), ui->comboProfile->currentIndex(), ui->comboProcess->currentIndex());

    painter.end();

    // Set up image
    ui->labelDisplay->setPixmap(pixmap);
}


static void printBegin(QPrinter &printer, QPainter &painter, QString const &fileName)
{
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageMargins(QMarginsF(0.0, 0.0, 0.0, 0.0), QPageLayout::Millimeter);

    printer.setDocName(fileName);
    printer.setOutputFileName(fileName);

    painter.begin(&printer);
}


static void printPageSetup(QPrinter &printer, QPainter &painter, bool isNewPage, QRect &rect)
{
    rect = QRect(0, 0, 1754 - 1, 1240 - 1); // paper A4

    if (isNewPage)
    {
        printer.newPage();
    }

    const auto pageLayout = printer.pageLayout();
    const auto pageRect = pageLayout.paintRectPixels(printer.resolution());
    const auto paperRect = pageLayout.fullRectPixels(printer.resolution());

    double xscale = pageRect.width() / double(rect.width());
    double yscale = pageRect.height() / double(rect.height());
    double scale = qMin(xscale, yscale);

    painter.resetTransform();
    painter.translate(pageRect.x() + paperRect.width() / 2.,
                      pageRect.y() + paperRect.height() / 2.);
    painter.scale(scale, scale);
    painter.translate(- rect.width() / 2., - rect.height() / 2.);

    const auto fontSize = 5.5;

    auto font = painter.font();
    font.setPointSizeF(fontSize / scale);
    painter.setFont(font);
}


static void printEnd(QPrinter &printer, QPainter &painter, QWidget *parent, QString const & parentTitle)
{
    painter.end();

    QPrintDialog dialog(&printer, parent);
    dialog.exec();

    auto fileName = printer.outputFileName();
    auto newFileName = QString("%1.pdf").arg(fileName);
    QFile::remove(newFileName);
    QFile::rename(fileName, newFileName);

    QMessageBox::information(parent, parentTitle, QFileInfo(newFileName).absoluteFilePath());
}


void WindowAnalyse::on_buttonPrint_clicked()
{
    QPrinter printer(QPrinter::PrinterResolution);
    QPainter painter;

    auto fileName = QString("%1 %2 %3 %4")
                        .arg(ui->comboModel->currentText(), ui->comboSide->currentText(), ui->comboProfile->currentText(), ui->comboProcess->currentText());

    printBegin(printer, painter, fileName);

    QRect rect;
    printPageSetup(printer, painter, false, rect);

    drawData(painter, rect, ui->comboModel->currentIndex(), ui->comboSide->currentIndex(), ui->comboProfile->currentIndex(), ui->comboProcess->currentIndex());

    printEnd(printer, painter, this, defaultTitle);
}


void WindowAnalyse::on_comboModel_activated(int)
{
    refreshData();
}


void WindowAnalyse::on_comboSide_activated(int)
{
    refreshData();
}


void WindowAnalyse::on_comboProcess_activated(int)
{
    refreshData();
}


void WindowAnalyse::on_comboProfile_activated(int)
{
    refreshData();
}


void WindowAnalyse::on_spinValuesLimit_valueChanged(int)
{
    refreshData();
}


void WindowAnalyse::on_comboTimestamp_activated(int)
{
    refreshData();
}


void WindowAnalyse::on_spinSigmaLevel_valueChanged(double)
{
    refreshData();
}


void WindowAnalyse::on_buttonAutoPrint_clicked()
{
    autoPrint();
}


static int extractSubIndex(int &index, int subLimit)
{
    int subIndex = index % subLimit;
    index = index / subLimit;
    return (subIndex);
}


void WindowAnalyse::autoPrint()
{
    QPrinter printer(QPrinter::PrinterResolution);
    QPainter painter;
    auto fileName = QString("Report Sill Seal");

    printBegin(printer, painter, fileName);

    int pageIndex = 0;
    while (true)
    {
        auto stepIndex = pageIndex;

        auto indexProcess = extractSubIndex(stepIndex, ui->comboProcess->count());
        auto indexProfile = extractSubIndex(stepIndex, ui->comboProfile->count());
        auto indexSide = extractSubIndex(stepIndex, ui->comboSide->count());
        auto indexModel = extractSubIndex(stepIndex, ui->comboModel->count());

        if (stepIndex > 0)
        {
            break;
        }

        auto isNewPage = (pageIndex > 0);

        QRect rect;
        printPageSetup(printer, painter, isNewPage, rect);

        drawData(painter, rect, indexModel, indexSide, indexProfile, indexProcess);

        ++pageIndex;
    }

    printEnd(printer, painter, this, defaultTitle);
}


void WindowAnalyse::on_checkDrawHistogram_stateChanged(int state)
{
    ui->spinHistogramColumns->setEnabled(state != 0);
    refreshData();
}


void WindowAnalyse::on_spinHistogramColumns_valueChanged(int)
{
    refreshData();
}


void WindowAnalyse::on_checkDrawColoredRanges_stateChanged(int)
{
    refreshData();
}

