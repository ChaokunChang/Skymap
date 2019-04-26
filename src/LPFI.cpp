#include "LPFI.h"
#include "MyFunctions.h"
#include "Star.h"
#include <QString>
#include <QFile>
#include <QTextStream>

bool mag_cmp(StarPoint& a,StarPoint& b)
{
    return a.magnitude>b.magnitude;
}

LPFI::LPFI(std::vector<StarPoint> navStarMap,double r=6, int m=200,int n=120,double f=0.004):navStarTable(navStarMap),r(r),m(m),n(n),focal_length(f)
{
    qDebug()<<"load sky complete!"<<endl;
    load();
}

void LPFI::load()
{
    QFile file("LPF.csv");
    if(!file.open(QIODevice::ReadOnly))
        init();
    else {
        LPFT.clear();
        QTextStream in(&file);
        while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split(',');
                int ridx=fields[0].toInt();
                vector<int> v;
                for(int i=1;i!=fields.size();i++)
                {
                    v.push_back(fields[i].toInt());
                }
                LPFT.push_back(v);
        }
    }
    qDebug()<<"load feature complete!"<<endl;
    for(vector<vector<int> >::iterator it=LPFT.begin();it!=LPFT.end();it++)
    {
        it->insert(it->end(),it->begin(),it->end());
    }
    sort(navStarTable.begin(),navStarTable.end(),mag_cmp);
}

void LPFI::init()
{
    LPFT.clear();
    for(vector<StarPoint>::iterator it=navStarTable.begin();it!=navStarTable.end();it++)
    {
        qDebug()<<"calculating feature of"<<it->index<<endl;
        vector<int> v=calc_LPF(navStarTable,*it,true);
        LPFT.push_back(v);
    }
    QFile file("LPF.csv");
    if (!file.open(QIODevice::WriteOnly))
        throw "File Not Found!";
    QTextStream out(&file);
    int k=0;
    for(vector<vector<int> >::iterator it=LPFT.begin();it!=LPFT.end();it++,k++)
    {
        out<<QString::number(k);
        for(vector<int>::iterator its=it->begin();its!=it->end();its++)
        {
            out<<','<<*its;
        }
        out<<'\n';
    }
}

int LPFI::find(std::vector<StarPoint>& ObserveStarTable,StarPoint target)
{
    vector<int> v=calc_LPF(ObserveStarTable,target,false);
    for(vector<StarPoint>::iterator it=navStarTable.begin();it!=navStarTable.end();it++)
    {
        if(compare(LPFT[it->index],v))
        {
            return it->index;
        }
    }
    return -1;
}

int LPFI::efind(std::vector<StarPoint>& ObserveStarTable,StarPoint target)
{
    vector<int> v=calc_LPF(ObserveStarTable,target,true);
    for(vector<StarPoint>::iterator it=navStarTable.begin();it!=navStarTable.end();it++)
    {
        if(compare(LPFT[it->index],v))
        {
            return it->index;
        }
    }
    return -1;
}

vector<int> LPFI::calc_LPF(std::vector<StarPoint>& StarTable,StarPoint target,bool is_star)
{
    vector<StarPoint> neighbour;
    for(vector<StarPoint>::iterator it=StarTable.begin();it!=StarTable.end();it++)
    {
        if(it->index!=target.index&&(is_star?getSphereAD(it->x,it->y,target.x,target.y)<r:getSpotAD(it->x,it->y,target.x,target.y,focal_length)<r))
        {
            neighbour.push_back(*it);
        }
    }
    bool pic[m][n];
    for(int i=0;i!=m;i++)
    {
        for(int j=0;j!=n;j++)
        {
            pic[i][j]=false;
        }
    }
    for(vector<StarPoint>::iterator it=neighbour.begin();it!=neighbour.end();it++)
    {
        if(is_star)
        {
            pair<double,double> tspot=star2spot(it->x,it->y,target.x,target.y,0,focal_length);
            pair<double,double> spot=LPT(tspot.first,tspot.second);
            pic[int(spot.first/(360/double(m)))][int(spot.second/(r/n))]=true;
        }
        else {
            pair<double,double> spot=LPT(it->x,it->y);
            pic[int(spot.first/(360/double(m)))][int(spot.second/(r/n))]=true;
        }
    }
    int tmpvec[m*2];
    for(int i=0;i!=m*2;i++)
    {
        tmpvec[i]=0;
    }
    for(int i=0;i!=m;i++)
    {
        for(int j=0;j!=n;j++)
        {
            if(pic[i][j]==true)
            {
                tmpvec[i]=j;
                break;
            }
        }
    }
    int fz;
    vector<int> v;
    for(fz=0;fz<m&&tmpvec[fz]==0;fz++)
    {
        tmpvec[m+fz]=0;
    }
    for(int i=fz;i<m+fz;)
    {
        v.push_back(tmpvec[i]);
        if(i+1>=m+fz||tmpvec[i+1]!=0)
        {
            v.push_back(0);
            i++;
            continue;
        }
        else {
            int tc;
            for(tc=1;i+tc<m+fz&&tmpvec[i+tc]==0;tc++);
            v.push_back(tc);
            i+=tc;
        }
    }
    return v;
}

vector<pair<int,int> > calc_next(vector<int>& v)
{
    vector<pair<int,int> > next;
    next.resize(v.size());
    next[0]={-2,0};
    int k=-2,ec=0;
    for(int q=2;q<v.size();q+=2)
    {
        while(k>-2&&abs(v[k+2]-v[q])>1)
        {
            if(ec<=2)
            {
                ec++;
            }
            else {
                ec=next[k].second;
                k=next[k].first;
            }
        }
        if(abs(v[k+2]-v[q])<=1)
        {
            k+=2;
        }
        next[q]={k,ec};
    }
    return next;
}

bool compare(vector<int>& v,vector<int>& p)
{
    vector<pair<int,int> > next=calc_next(p);
    int k=-2,ec=0,sp=0;
    for(int i=1;i<p.size();i+=2)
    {
        sp+=p[i];
    }
    for(int i=0;i<v.size();i+=2)
    {
        while(k>-2&&abs(p[k+2]-v[i])>1)
        {
            if(ec<=2)
            {
                ec++;
            }
            else {
                ec=next[k].second;
                k=next[k].first;
            }
        }
        if(abs(p[k+2]-v[i])<=1)
        {
            k+=2;
        }
        if(k==p.size()-2)
        {
            int vs=0;
            for(int j=i-p.size()+3;j<=i+1;j+=2)
            {
                vs+=v[j];
            }
            if(abs(vs-sp)<=1)
            {
                return true;
            }
            else {
                k=-2;
                i+=2;
            }
        }
    }
    return false;
}
