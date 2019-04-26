#ifndef LPFI_H
#define LPFI_H
#include <vector>
#include "Star.h"
using namespace std;
class LPFI
{
public:
    LPFI(std::vector<StarPoint>,double,int,int,double);
    int find(std::vector<StarPoint>&,StarPoint);
    int efind(std::vector<StarPoint>&,StarPoint);
private:
    double r,focal_length;
    int m,n;
    vector<StarPoint> navStarTable;
    vector<vector<int> > LPFT;
    void load();
    void init();
    vector<int> calc_LPF(vector<StarPoint>&,StarPoint,bool);
};

vector<pair<int,int> > calc_next(vector<int>&);
bool compare(vector<int>&,vector<int>&);
#endif // LPFI_H
