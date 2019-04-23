#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include "Star.h"
struct RCandidate{
    int idx;
    int rconf;
    int cconf;
    int nconf;
    bool operator<(RCandidate that)
    {
        return rconf>that.rconf||(rconf==that.rconf&&cconf<that.cconf)||(rconf==that.rconf&&cconf==that.cconf&&nconf>that.nconf);
    }
};

class RCFI
{
public:
    RCFI(std::vector<StarPoint>, double, int,double);
	~RCFI();
	void init();
    void load();
    int find(std::vector<StarPoint>&,StarPoint);
    int sfind(std::vector<StarPoint>&,StarPoint);
    double focal_length;
private:
    std::vector<StarPoint> NavStarTable;
	std::vector<std::set<int>> RadiusFeatureTable;
	std::map<int,int> CyclicFeatureTable;
    std::map<int,int> NeighbourSizeTable;
	double Rr;
    int Nq;
};

