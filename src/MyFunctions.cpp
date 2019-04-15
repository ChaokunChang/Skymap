//
// Created by 70700 on 2019/3/3.
//
#include "MyFunctions.h"

double cal_dis(double x1,double y1,double x2,double y2){
    //return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    double dx = abs(x1-x2);
    double dy = abs(y1-y2);
    if(dx>180) dx = 360-dx;
    return sqrt( pow(dx,2) + pow(dy,2) );
}

bool between(double target, double left, double right){
    if(target>=left && target<=right) return true;
    else return false;
}

double getSpotAD(double x1, double y1, double x2, double y2, double f)
{
    return acos((x1*x2 + y1 * y2 + f * f) / sqrt((x1*x1 + y1 * y1 + f * f)*(x2*x2 + y2 * y2 + f * f)));
}


int random_int(int l,int r){
//    struct timeb time_seed;
//    ftime(&time_seed);
//    srand(time_seed.time*1000 + time_seed.millitm);
    return l + rand()%(r-l);
}

double random_double(double l, double r){
//    static struct timeb time_seed;
//    ftime(&time_seed);
//    srand(time_seed.time*1000 + time_seed.millitm);
    return l + rand()/static_cast<double>(RAND_MAX/(r-l));
}


