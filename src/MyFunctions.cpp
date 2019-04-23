//
// Created by 70700 on 2019/3/3.
//
#include "MyFunctions.h"


double sacos(double x)
{
    return acos(std::min(1.0, std::max(-1.0, x)));
}

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
    double ret=sacos((x1*x2 + y1 * y2 + f * f) / sqrt((x1*x1 + y1 * y1 + f * f)*(x2*x2 + y2 * y2 + f * f)));
    return ret*180/M_PI;
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

double getSphereAD(double x1, double y1, double x2, double y2)
{
    double cosX1=cos(x1*M_PI/180),cosY1=cos(y1*M_PI/180),sinX1=sin(x1*M_PI/180),sinY1=sin(y1*M_PI/180);
    double cosX2=cos(x2*M_PI/180),cosY2=cos(y2*M_PI/180),sinX2=sin(x2*M_PI/180),sinY2=sin(y2*M_PI/180);
    double ret=acos((cosX1*cosY1*cosX2*cosY2 + cosX1*sinY1*cosX2*sinY2 + sinX1*sinX2));
    return ret*180/M_PI;
}

double getSphereAngle(double x0, double y0, double x1, double y1, double x2, double y2)
{
    double ret=sacos((pow(getSphereAD(x0, y0, x1, y1)*M_PI/180,2) + pow(getSphereAD(x0, y0, x2, y2)*M_PI/180,2) - pow(getSphereAD(x1, y1, x2, y2)*M_PI/180,2)) / 2 * getSphereAD(x0, y0, x1, y1)*M_PI/180*getSphereAD(x0, y0, x2, y2)*M_PI/180);
    return ret*180/M_PI;
}

double getSpotAngle(double x0, double y0, double x1, double y1, double x2, double y2,double focal_length)
{
    double ret=sacos((pow(getSpotAD(x0, y0, x1, y1,focal_length)*M_PI/180,2) + pow(getSpotAD(x0, y0, x2, y2,focal_length)*M_PI/180,2) - pow(getSpotAD(x1, y1, x2, y2,focal_length)*M_PI/180,2)) / 2 * getSpotAD(x0, y0, x1, y1,focal_length)*M_PI/180*getSpotAD(x0, y0, x2, y2,focal_length)*M_PI/180);
    return ret*180/M_PI;
}
