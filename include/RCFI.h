#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include "Star.h"

class RCFI
{
public:
    RCFI(std::vector<StarPoint>, double, int,double);
	~RCFI();
	void init();
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

