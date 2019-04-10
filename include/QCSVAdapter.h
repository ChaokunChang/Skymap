//Qt version of CSVAdapter
#ifndef QCSV_H
#define QCSV_H
#include <QFile>
#include <QString>
#include "Star.h"
#include <vector>

class QCSVAdapter
{
public:
    QCSVAdapter(QString);
    ~QCSVAdapter();
    std::vector<StarPoint> getRecords();
    int writeRecords(std::vector<StarPoint>);
    int appendNewRecord(StarPoint);
private:
    QString fileName;
};

#endif
