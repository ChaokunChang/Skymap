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
    bool operator<(RCandidate that)
    {
        return rconf>that.rconf||(rconf==that.rconf&&cconf<that.cconf)||(rconf==that.rconf&&cconf==that.cconf&&idx<that.idx);
    }
};

class RCFI
{
public:
    RCFI(std::vector<StarPoint>, double, int,double);
	~RCFI();
	void init();
    void load();
    int find(std::vector<StarPoint>,StarPoint);
    int efind(std::vector<StarPoint>,StarPoint);
private:
    std::vector<StarPoint> NavStarTable;
	std::vector<std::set<int>> RadiusFeatureTable;
	std::map<int,int> CyclicFeatureTable;
	double Rr;
	int Nq;
    double focal_length;
};

