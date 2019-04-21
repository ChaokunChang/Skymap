#include "RCFI.h"
#include <string>
#include <vector>
#include <exception>
#include <set>
#include <algorithm>
#include <map>
#include "MyFunctions.h"
#include "Star.h"

RCFI::RCFI(std::vector<StarPoint> navStarMap,double Rr=10, int Nq=200,double f=4):NavStarTable(navStarMap),Rr(Rr),Nq(Nq),focal_length(f)
{
    RadiusFeatureTable.resize(Nq);
    qDebug()<<"sky load successfully."<<endl;
}


RCFI::~RCFI()
{
}

void RCFI::init()
{
    for (std::vector<StarPoint>::iterator it = NavStarTable.begin(); it != NavStarTable.end(); it++)
	{
        std::vector<StarPoint> neighbour;
        for (std::vector<StarPoint>::iterator its = NavStarTable.begin(); its != NavStarTable.end(); its++)
		{	
            if (it->index != its->index && getSphereAD(it->x,it->y,its->x,its->y)<=Rr)
			{
                RadiusFeatureTable[(int)(getSphereAD(it->x, it->y, its->x, its->y) / (Rr/Nq))].insert(it->index);
				neighbour.push_back(*its);
			}
		}
        qDebug()<<"successfully get radius feature."<<it->index<<"neighbour size:"<<neighbour.size()<<endl;
		double minAngle=500;
        StarPoint minPosStar;
        for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
		{
            for (std::vector<StarPoint>::iterator itp = its+1; itp != neighbour.end(); itp++)
			{
                double tminAngle = getSphereAngle(it->x, it->y, its->x, its->y, itp->x, itp->y);
                if (tminAngle<minAngle)
				{
                    minAngle=tminAngle;
                    minPosStar = ((its->y - it->y) / (its->x - it->x)) < ((itp->y - it->y) / (itp->x - it->x)) ? *its: *itp;
				}
			}
		}
        qDebug()<<"successfully min pos star"<<minPosStar.index<<"."<<it->index<<endl;
        std::pair<int,int> cf = { it->index,0 };
		int tmpArr[7] = { 0 };
        for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
		{
            if (its->index != minPosStar.index)
			{
                if (tmpArr[(int)(getSphereAngle(it->x, it->y, its->x, its->y, minPosStar.x, minPosStar.y) / 2 * M_PI)]==0)
				{
                    tmpArr[(int)(getSphereAngle(it->x, it->y, its->x, its->y, minPosStar.x, minPosStar.y) / 2 * M_PI)] = 1;
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
}

int RCFI::find(std::vector<StarPoint> ObserveStarTable,StarPoint target)
{
	std::map<int, int> starMap;
    std::vector<StarPoint> neighbour;
    for (std::vector<StarPoint>::iterator it = ObserveStarTable.begin(); it != ObserveStarTable.end(); it++)
	{
        if (it->index != target.index && getSpotAD(it->x, it->y, target.x, target.y,focal_length) <= Rr)
		{
            std::set<int> tmpSet = RadiusFeatureTable[(int)(getSpotAD(it->x, it->y, target.x, target.y,this->focal_length) / (Rr/Nq))];
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
	std::vector<int> canStar;
	for (std::map<int, int>::iterator itm = starMap.lower_bound(starMap.rbegin()->second); itm != starMap.end(); itm++)
	{
		canStar.push_back(itm->first);
	}
	double minAngle = 500;
    StarPoint minPosStar;
    for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
	{
        for (std::vector<StarPoint>::iterator itp = its+1; itp != neighbour.end(); itp++)
		{
            double tminAngle = getSpotAngle(target.x, target.y, its->x, its->y, itp->x, itp->y,focal_length);
            if (tminAngle < minAngle)
			{
                minAngle=tminAngle;
                minPosStar = ((its->y - target.y) / (its->x - target.x)) < ((itp->y - target.y) / (itp->x - target.x)) ? *its : *itp;
			}
		}
	}
    qDebug()<<"successfully min pos star"<<minPosStar.index<<"."<<endl;
	int cf = 0;
	int tmpArr[7] = { 0 };
    for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
	{
        if (its->index != minPosStar.index)
		{
            if (tmpArr[(int)(getSpotAngle(target.x, target.y, its->x, its->y, minPosStar.x, minPosStar.y,focal_length) / 2 * M_PI)] == 0)
			{
                tmpArr[(int)(getSpotAngle(target.x, target.y, its->x, its->y, minPosStar.x, minPosStar.y,focal_length) / 2 * M_PI)] = 1;
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
	for (std::vector<int>::iterator it = canStar.begin(); it != canStar.end(); it++)
	{
		if (CyclicFeatureTable.find(*it) != CyclicFeatureTable.end() && CyclicFeatureTable.find(*it)->second != cf)
		{
			canStar.erase(it);
			it--;
		}
	}
    qDebug()<<"successfully get cyclic feature."<<endl;
	if (canStar.size() == 1)
	{
		return canStar[0];
	}
	else
	{
		return -1;
	}
}
int RCFI::efind(std::vector<StarPoint> ObserveStarTable,StarPoint target)
{
    std::map<int, int> starMap;
    std::vector<StarPoint> neighbour;
    for (std::vector<StarPoint>::iterator it = ObserveStarTable.begin(); it != ObserveStarTable.end(); it++)
    {
        if (it->index != target.index && getSphereAD(it->x, it->y, target.x, target.y) <= Rr)
        {
            std::set<int> tmpSet = RadiusFeatureTable[(int)(getSphereAD(it->x, it->y, target.x, target.y) / (Rr/Nq))];
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
    std::vector<int> canStar;
    for (std::map<int, int>::iterator itm = starMap.lower_bound(starMap.rbegin()->second); itm != starMap.end(); itm++)
    {
        canStar.push_back(itm->first);
    }
    double minAngle = 500;
    StarPoint minPosStar;
    for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
    {
        for (std::vector<StarPoint>::iterator itp = its+1; itp != neighbour.end(); itp++)
        {
            double tminAngle = getSphereAngle(target.x, target.y, its->x, its->y, itp->x, itp->y);
            if (tminAngle < minAngle)
            {
                minAngle=tminAngle;
                minPosStar = ((its->y - target.y) / (its->x - target.x)) < ((itp->y - target.y) / (itp->x - target.x)) ? *its : *itp;
            }
        }
    }
    qDebug()<<"successfully min pos star"<<minPosStar.index<<"."<<endl;
    int cf = 0;
    int tmpArr[7] = { 0 };
    for (std::vector<StarPoint>::iterator its = neighbour.begin(); its != neighbour.end(); its++)
    {
        if (its->index != minPosStar.index)
        {
            if (tmpArr[(int)(getSphereAngle(target.x, target.y, its->x, its->y, minPosStar.x, minPosStar.y) / 2 * M_PI)] == 0)
            {
                tmpArr[(int)(getSphereAngle(target.x, target.y, its->x, its->y, minPosStar.x, minPosStar.y) / 2 * M_PI)] = 1;
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
    for (std::vector<int>::iterator it = canStar.begin(); it != canStar.end(); it++)
    {
        if (CyclicFeatureTable.find(*it) != CyclicFeatureTable.end() && CyclicFeatureTable.find(*it)->second != cf)
        {
            canStar.erase(it);
            it--;
        }
    }
    qDebug()<<"successfully get cyclic feature."<<endl;
    if (canStar.size() == 1)
    {
        return canStar[0];
    }
    else
    {
        return -1;
    }
}
