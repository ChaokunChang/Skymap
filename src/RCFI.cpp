#include "RCFI.h"
#include <string>
#include <vector>
#include <exception>
#include <set>
#include <algorithm>
#include <map>
#include "MyFunctions.h"
#include "Star.h"
#include <QString>
#include <QFile>
#include <QTextStream>

RCFI::RCFI(std::vector<StarPoint> navStarMap,double Rr=10, int Nq=200,double f=4):NavStarTable(navStarMap),Rr(Rr),Nq(Nq),focal_length(f)
{
    RadiusFeatureTable.resize(Nq);
    load();
    qDebug()<<"sky load successfully."<<endl;
}


RCFI::~RCFI()
{
}

void RCFI::load()
{
    QFile Rfile("RFI.csv");
    QFile Cfile("CFI.csv");
    QFile Nfile("NSI.csv");
    if (!Rfile.open(QIODevice::ReadOnly)||!Cfile.open(QIODevice::ReadOnly)||!Nfile.open(QIODevice::ReadOnly))
        init();
    else {
        RadiusFeatureTable.clear();
        RadiusFeatureTable.resize(Nq);
        QTextStream Rin(&Rfile);
        while (!Rin.atEnd()) {
                QString line = Rin.readLine();
                QStringList fields = line.split(',');
                int ridx=fields[0].toInt();
                for(int i=1;i!=fields.size();i++)
                {
                    RadiusFeatureTable[ridx].insert(fields[i].toInt());
                }
        }
        CyclicFeatureTable.clear();
        QTextStream Cin(&Cfile);
        while (!Cin.atEnd()) {
                QString line = Cin.readLine();
                QStringList fields = line.split(',');
                CyclicFeatureTable.insert({fields[0].toInt(),fields[1].toInt()});
        }
        NeighbourSizeTable.clear();
        QTextStream Nin(&Nfile);
        while (!Nin.atEnd()) {
                QString line = Nin.readLine();
                QStringList fields = line.split(',');
                NeighbourSizeTable.insert({fields[0].toInt(),fields[1].toInt()});
        }
    }
}

void RCFI::init()
{
    for (std::vector<StarPoint>::iterator it = NavStarTable.begin(); it != NavStarTable.end(); it++)
    {
        std::vector<StarPoint> neighbour;
        for (std::vector<StarPoint>::iterator its = NavStarTable.begin(); its != NavStarTable.end(); its++)
        {
            if (it->index != its->index && getSphereAD(it->x,it->y,its->x,its->y)<Rr)
            {
                int ridx=(int)(getSphereAD(it->x, it->y, its->x, its->y) / (Rr/Nq));
                RadiusFeatureTable[ridx].insert(it->index);
                neighbour.push_back(*its);
            }
        }
        qDebug()<<"successfully get radius feature."<<it->index<<"neighbour size:"<<neighbour.size()<<endl;
            NeighbourSizeTable.insert({it->index,neighbour.size()});
            double minAngle=500;
            StarPoint minPosStar;
            for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
            {
                for (std::vector<StarPoint>::iterator itp = neighbour.begin(); itp != neighbour.end(); itp++)
                {
                    if(its!=itp)
                    {
                        double tminAngle = abs(getSphereAngle(it->x, it->y, its->x, its->y, itp->x, itp->y));
                        if (tminAngle<minAngle)
                        {
                            minAngle=tminAngle;
                            minPosStar = ((its->y - it->y) / (its->x - it->x)) < ((itp->y - it->y) / (itp->x - it->x)) ? *its: *itp;
                        }
                    }

                }
            }
            qDebug()<<"successfully min pos star"<<minPosStar.index<<"."<<it->index<<endl;
            std::pair<int,int> cf = { it->index,0 };
            int tmpArr[8] = { 0 };
            for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
            {
                if (its->index != minPosStar.index)
                {
                    int tidx=(int)(getSphereAngle(it->x, it->y, its->x, its->y, minPosStar.x, minPosStar.y)/45);
                    if (tmpArr[tidx]==0)
                    {
                        tmpArr[tidx] = 1;
                    }
                }
            }
            for (int i = 0; i != 8; i++)
            {
                int t=0;
                for (int j = i; j != i + 8; j++)
                {
                    t += tmpArr[j % 8] * pow(2, j - i);
                }
                cf.second = std::max(cf.second, t);
            }
            CyclicFeatureTable.insert(cf);
        qDebug()<<"successfully get cyclic feature."<<it->index<<endl;
    }
    QFile Rfile("RFI.csv");
    if (!Rfile.open(QIODevice::WriteOnly))
        throw "File Not Found!";
    QTextStream Rout(&Rfile);
    int k=0;
    for (std::vector<std::set<int> >::iterator it=RadiusFeatureTable.begin();it!=RadiusFeatureTable.end();it++,k++) {
        Rout<<k;
        int i=0;
        for (std::set<int>::iterator its=it->begin();its!=it->end();its++,i++) {
            Rout<<','<<*its;
        }
        Rout<<'\n';
    }
    QFile Cfile("CFI.csv");
    if (!Cfile.open(QIODevice::WriteOnly))
        throw "File Not Found!";
    QTextStream Cout(&Cfile);
    for (std::map<int,int>::iterator it=CyclicFeatureTable.begin();it!=CyclicFeatureTable.end();it++) {
        Cout<<it->first<<','<<it->second<<'\n';
    }
    QFile Nfile("NSI.csv");
    if (!Nfile.open(QIODevice::WriteOnly))
         throw "File Not Found!";
    QTextStream Nout(&Nfile);
    for (std::map<int,int>::iterator it=NeighbourSizeTable.begin();it!=NeighbourSizeTable.end();it++) {
         Nout<<it->first<<','<<it->second<<'\n';
    }
}

int RCFI::find(std::vector<StarPoint>& ObserveStarTable,StarPoint target)
{
    std::map<int, int> starMap;
    std::vector<StarPoint> neighbour;
    for (std::vector<StarPoint>::iterator it = ObserveStarTable.begin(); it != ObserveStarTable.end(); it++)
    {
        if (it->index != target.index && getSpotAD(it->x, it->y, target.x, target.y,focal_length) < Rr)
        {
            int ridx=(int)(getSpotAD(it->x, it->y, target.x, target.y,this->focal_length) / (Rr/Nq));
            std::set<int> tmpSet = RadiusFeatureTable[ridx];
            neighbour.push_back(*it);
            for (std::set<int>::iterator its = tmpSet.begin(); its != tmpSet.end(); its++)
            {
                if (starMap.find(*its) != starMap.end())
                {
                    starMap[*its]++;
                }
                else
                {
                    starMap.insert(std::pair<int, int>(*its, 1));
                }
            }
        }
    }
    qDebug()<<"successfully get radius feature."<<endl;
    std::vector<RCandidate> canStar;
    if(starMap.empty())
        return -1;
    for (std::map<int, int>::iterator itm = starMap.begin(); itm != starMap.end(); itm++)
    {
        canStar.push_back({itm->first,itm->second,256,NeighbourSizeTable[itm->first]});
    }
    double minAngle = 500;
    StarPoint minPosStar;
    for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
    {
        for (std::vector<StarPoint>::iterator itp = neighbour.begin(); itp != neighbour.end(); itp++)
        {
            if(its!=itp)
            {
                double tminAngle = abs(getSpotAngle(target.x, target.y, its->x, its->y, itp->x, itp->y,focal_length));
                if (tminAngle < minAngle)
                {
                    minAngle=tminAngle;
                    minPosStar = ((its->y - target.y) / (its->x - target.x)) < ((itp->y - target.y) / (itp->x - target.x)) ? *its : *itp;
                }
            }

        }
    }
    qDebug()<<"successfully min pos star"<<minPosStar.index<<"."<<endl;
    int cf = 0;
    int tmpArr[8] = { 0 };
    for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
    {
        if (its->index != minPosStar.index)
        {
            int tidx=(int)(getSpotAngle(target.x, target.y, its->x, its->y, minPosStar.x, minPosStar.y,focal_length) /45);
            if (tmpArr[tidx] == 0)
            {
                tmpArr[tidx] = 1;
            }
        }
    }
    for (int i = 0; i != 8; i++)
    {
        int t = 0;
        for (int j = i; j != i + 8; j++)
        {
            t += tmpArr[j % 8] * pow(2, j - i);
        }
        cf = std::max(cf, t);
    }
    for (std::vector<RCandidate>::iterator it = canStar.begin(); it != canStar.end(); it++)
    {
        if (CyclicFeatureTable.find(it->idx) != CyclicFeatureTable.end())
        {
            it->cconf=abs(CyclicFeatureTable.find(it->idx)->second-cf);
        }
    }
    qDebug()<<"successfully get cyclic feature."<<endl;
    sort(canStar.begin(),canStar.end());
    if(canStar.empty())
    {
        return -1;
    }
    else {
        return canStar[0].idx;
    }
}

int RCFI::sfind(std::vector<StarPoint>& ObserveStarTable,StarPoint target)
{
    std::map<int, int> starMap;
    std::vector<StarPoint> neighbour;
    for (std::vector<StarPoint>::iterator it = ObserveStarTable.begin(); it != ObserveStarTable.end(); it++)
    {
        if (it->index != target.index && getSphereAD(it->x, it->y, target.x, target.y) < Rr)
        {
            int ridx=(int)(getSphereAD(it->x, it->y, target.x, target.y) / (Rr/Nq));
            std::set<int> tmpSet = RadiusFeatureTable[ridx];
            neighbour.push_back(*it);
            for (std::set<int>::iterator its = tmpSet.begin(); its != tmpSet.end(); its++)
            {
                if (starMap.find(*its) != starMap.end())
                {
                    starMap[*its]++;
                }
                else
                {
                    starMap.insert(std::pair<int, int>(*its, 1));
                }
            }
        }
    }
    qDebug()<<"successfully get radius feature."<<endl;
    std::vector<RCandidate> canStar;
    if(starMap.empty())
        return -1;
    for (std::map<int, int>::iterator itm = starMap.begin(); itm != starMap.end(); itm++)
    {
        canStar.push_back({itm->first,itm->second,256,NeighbourSizeTable[itm->first]});
    }
    double minAngle = 500;
    StarPoint minPosStar;
    for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
    {
        for (std::vector<StarPoint>::iterator itp = neighbour.begin(); itp != neighbour.end(); itp++)
        {
            if(its!=itp)
            {
                double tminAngle = abs(getSphereAngle(target.x, target.y, its->x, its->y, itp->x, itp->y));
                if (tminAngle < minAngle)
                {
                    minAngle=tminAngle;
                    minPosStar = ((its->y - target.y) / (its->x - target.x)) < ((itp->y - target.y) / (itp->x - target.x)) ? *its : *itp;
                }
            }

        }
    }
    qDebug()<<"successfully min pos star"<<minPosStar.index<<"."<<endl;
    int cf = 0;
    int tmpArr[8] = { 0 };
    for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
    {
        if (its->index != minPosStar.index)
        {
            int tidx=(int)(getSphereAngle(target.x, target.y, its->x, its->y, minPosStar.x, minPosStar.y) /45);
            if (tmpArr[tidx] == 0)
            {
                tmpArr[tidx] = 1;
            }
        }
    }
    for (int i = 0; i != 8; i++)
    {
        int t = 0;
        for (int j = i; j != i + 8; j++)
        {
            t += tmpArr[j % 8] * pow(2, j - i);
        }
        cf = std::max(cf, t);
    }
    for (std::vector<RCandidate>::iterator it = canStar.begin(); it != canStar.end(); it++)
    {
        if (CyclicFeatureTable.find(it->idx) != CyclicFeatureTable.end())
        {
            it->cconf=abs(CyclicFeatureTable.find(it->idx)->second-cf);
        }
    }
    qDebug()<<"successfully get cyclic feature."<<endl;
    sort(canStar.begin(),canStar.end());
    if(canStar.empty())
    {
        return -1;
    }
    else {
        return canStar[0].idx;
    }
}
