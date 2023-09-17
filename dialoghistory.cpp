#include "dialoghistory.h"
#include "ui_dialoghistory.h"
#include <QFile>
#include <QDir>
#include "globals.h"


const char delimiter = ';';
const char* dirHistory = "history";


DialogHistory::DialogHistory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHistory)
{
    ui->setupUi(this);

    ui->tabWidget->setTabText(0, Globals::titlesSeals[0]);
    ui->tableWidget_1->setColumnCount(13);
    readHistory(0, ui->tableWidget_1);

    ui->tabWidget->setTabText(1, Globals::titlesSeals[1]);
    ui->tableWidget_2->setColumnCount(13);
    readHistory(1, ui->tableWidget_2);

    ui->tabWidget->setTabText(2, Globals::titlesSeals[2]);
    ui->tableWidget_3->setColumnCount(15);
    readHistory(2, ui->tableWidget_3);

    ui->tabWidget->setTabText(3, Globals::titlesSeals[3]);
    ui->tableWidget_4->setColumnCount(15);
    readHistory(3, ui->tableWidget_4);
}


DialogHistory::~DialogHistory()
{
    delete ui;
}


static QString getHistoryFilePath(int sealTypeId)
{
    return QString(dirHistory) + QDir::separator() + Globals::filesSeals.at(sealTypeId);
}


static bool createHistoryFile(int sealTypeId, QFile& file)
{
    QDir dir("");
    if (! dir.exists(dirHistory))
    {
        if (! dir.mkdir(dirHistory))
        {
            return (false);
        }
    }

    if (! file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
    {
        return (false);
    }

    QList<PointRange> pointsRanges;
    QStringList pointsTitles;

    if (Globals::isIndexSealMaxi(sealTypeId))
    {
        pointsRanges.append(Globals::rangesMaxiPointsP6);
        pointsRanges.append(Globals::rangesMaxiPointsP7);
        pointsTitles.append(Globals::pointsP6Titles.mid(0, Globals::rangesMaxiPointsP6.size()));
        pointsTitles.append(Globals::pointsP7Titles.mid(0, Globals::rangesMaxiPointsP7.size()));
    }
    else
    {
        pointsRanges.append(Globals::rangesStandardPointsP6);
        pointsRanges.append(Globals::rangesStandardPointsP7);
        pointsTitles.append(Globals::pointsP6Titles.mid(0, Globals::rangesStandardPointsP6.size()));
        pointsTitles.append(Globals::pointsP7Titles.mid(0, Globals::rangesStandardPointsP7.size()));
    }

    //row 1
    {
        QString line = Globals::titlesSeals.at(sealTypeId);
        line.append(delimiter);

        foreach (auto &pointTitle, pointsTitles)
        {
            line.append(delimiter);
            line.append(pointTitle);
        }

        line.append('\n');
        file.write(line.toUtf8());
    }

    //row 2
    {
        QString line;
        line.append(delimiter);

        foreach (auto &pointRange, pointsRanges)
        {
            line.append(delimiter);
            QString rangeText = QString("%1 \u00B1 %2 mm")
                                    .arg(pointRange.m_zero, 0, 'f', 1).arg(pointRange.m_error, 0, 'f', 1);
            line.append(rangeText);
        }

        line.append('\n');
        file.write(line.toUtf8());
    }

    //row 3
    {
        QString line = QString("Date & Time");
        line.append(delimiter);
        line.append(QString("Operator"));

        foreach (auto &pointRange, pointsRanges)
        {
            line.append(delimiter);
            QString rangeText = QString("%1 - %2 mm")
                                .arg(pointRange.m_zero-pointRange.m_error, 0, 'f', 1).arg(pointRange.m_zero+pointRange.m_error, 0, 'f', 1);
            line.append(rangeText);
        }

        line.append('\n');
        file.write(line.toUtf8());
    }

    file.close();

    return (true);
}


void DialogHistory::readHistory(int sealTypeId, QTableWidget *tableWidget)
{
    QFile file(getHistoryFilePath(sealTypeId));
    if (! file.exists())
    {
        createHistoryFile(sealTypeId, file);
    }

    if (! file.open(QFile::ReadOnly | QFile::Text | QFile::ExistingOnly))
    {
        return;
    }

    {
    //row 1 + check file
        QString textLine1 = QString::fromUtf8(file.readLine());
        auto lineItems1 = textLine1.trimmed().split(delimiter);

        if (lineItems1.count() != tableWidget->columnCount())
        {
            return;
        }

        if (lineItems1.at(0) != Globals::titlesSeals.at(sealTypeId))
        {
            return;
        }

        lineItems1[0].clear();

    //row 2
        QString textLine2 = QString::fromUtf8(file.readLine());
        auto lineItems2 = textLine2.trimmed().split(delimiter);

        if (lineItems2.count() != tableWidget->columnCount())
        {
            return;
        }

    //row 3
        QString textLine3 = QString::fromUtf8(file.readLine());
        auto lineItems3 = textLine3.trimmed().split(delimiter);

        if (lineItems3.count() != tableWidget->columnCount())
        {
            return;
        }

    //set header
        for (int index = 0; index < tableWidget->columnCount(); ++index)
        {
            QString title = QString("%1\n%2\n%3")
                                .arg(lineItems1.at(index), lineItems2.at(index), lineItems3.at(index));
            tableWidget->setHorizontalHeaderItem(index, new QTableWidgetItem(title));
        }

    }

    //rows points
    while (! file.atEnd())
    {
        QString textLine = QString::fromUtf8(file.readLine());

        auto lineItems = textLine.trimmed().split(delimiter);
        if (lineItems.size() != tableWidget->columnCount())
        {
            break;
        }

        int rowIndex = tableWidget->rowCount();
        tableWidget->insertRow(rowIndex);

        for (int colIndex = 0; colIndex < tableWidget->columnCount(); ++colIndex)
        {
            tableWidget->setItem(rowIndex, colIndex, new QTableWidgetItem(lineItems.at(colIndex)));
        }
    }

    file.close();
}


bool DialogHistory::appendToHistory(int sealTypeId, QString const& currentTime, QString const& currentOperator, QList<PointData> const& pointsData)
{
    QFile file(getHistoryFilePath(sealTypeId));

    if (! file.exists())
    {
        createHistoryFile(sealTypeId, file);
    }

    // fast check file
    {
        bool isFileValid = false;

        if (file.open(QFile::ReadOnly | QFile::Text | QFile::ExistingOnly))
        {
            QString textLine = QString::fromUtf8(file.readLine());
            if (textLine.startsWith(Globals::titlesSeals.at(sealTypeId)))
            {
                if (textLine.count(delimiter) == pointsData.size() + 2 - 1) // 2 == datetime + operator
                {
                    isFileValid = true;
                }
            }

            file.close();
        }

        if (! isFileValid)
        {
            return (false);
        }
    }

    // build line
    QString line = currentTime;
    line.append(delimiter);
    line.append(currentOperator);

    foreach (auto const& point, pointsData)
    {
        line.append(delimiter);
        if (point.m_isSetted)
        {
            line.append(QString::number(point.m_value, 'f', 1));
            if (! point.m_isValid)
            {
                line.append('*');
            }
        }
    }

    line.append('\n');

    // write line to file
    bool isWritten = false;

    if (file.open(QFile::WriteOnly | QFile::Text | QFile::ExistingOnly | QFile::Append))
    {
        QByteArray lineData = line.toUtf8();
        if (file.write(lineData) == lineData.size())
        {
            isWritten = true;
        }

        file.close();
    }

    return (isWritten);
}
