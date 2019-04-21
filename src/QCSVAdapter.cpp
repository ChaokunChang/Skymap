#include <QString>
#include <QFile>
#include <QTextStream>
#include <exception>
#include <vector>
#include "QCSVAdapter.h"

using namespace std;
QCSVAdapter::QCSVAdapter(QString FileName):fileName(FileName)
{
}

QCSVAdapter::~QCSVAdapter()
{
}

std::vector<StarPoint> QCSVAdapter::getRecords()
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        throw "File Not Found!";
    std::vector<StarPoint> starRecList;
    QTextStream in(&file);
    while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(',');
            StarPoint starRec(fields[0].toInt(),fields[1].toDouble(),fields[2].toDouble(),fields[3].toDouble());
            starRecList.push_back(starRec);
    }
    return starRecList;
}

int QCSVAdapter::writeRecords(std::vector<StarPoint> starRecList)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return -1;

    QTextStream out(&file);
    for (size_t i=0;i!=starRecList.size();i++) {
        StarPoint starRec = starRecList[i];
        out << starRec.index << ',' << starRec.x << ',' << starRec.y << ',' << starRec.magnitude << "\n";
    }
    return 0;
}

int QCSVAdapter::appendNewRecord(StarPoint starRec)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return -1;

    QTextStream out(&file);
    out << starRec.index << ',' << starRec.x << ',' << starRec.y << ',' << starRec.magnitude << "\n";
	return 0;
}
