//
// Created by 70700 on 2019/3/3.
//
#include "MyFunctions.h"

double cal_dis(const double& x1,const double& y1,const double& x2,const double& y2){
    //return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    double dx = abs(x1-x2);
    double dy = abs(y1-y2);
    if(dx>180) dx = 360-dx;
    return sqrt( pow(dx,2) + pow(dy,2) );
}

bool between(const double& target,const double& left,const double& right){
    if(target>=left && target<=right) return true;
    else return false;
}

double getSpotAD(const double& x1,const double& y1,const double& x2,const double& y2,const double& f)
{
    return acos((x1*x2 + y1 * y2 + f * f) / sqrt((x1*x1 + y1 * y1 + f * f)*(x2*x2 + y2 * y2 + f * f)));
}


int random_int(const int& l,const int& r){
    return l + rand()%(r-l);
}

size_t random_size_t(const size_t& l,const size_t& r){
    return size_t(random_int(int(l),int(r)));
}

double random_double(const double& l,const double& r){
    return l + (double(rand())/RAND_MAX)*(r-l);
}



