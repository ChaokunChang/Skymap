//
// Created by 70700 on 2019/3/3.
//

#ifndef SKYMAP_STAR_H
#define SKYMAP_STAR_H

#include "MyFunctions.h"
struct StarPoint{
    int index;
    double x; //0 -> 360
    double y; //-90 -> 90
    double magnitude;
    StarPoint():index(0),x(0.0),y(0.0),magnitude(0.0){}
    StarPoint(int i,double x, double y,double mag):index(i),x(x),y(y),magnitude(mag){}
    double Distance(const StarPoint &s){ return getSphereAD(x,y,s.x,s.y);}
    double Module(){ return  sqrt(pow(x,2)+pow(y,2));}
    bool XInRange(const StarPoint &center, double length);
    bool YInRange(const StarPoint &center, double width);
    bool PlaneInRange(const StarPoint &center, double length, double width);
    bool InRange(const StarPoint &center, double length, double width);
    void change_coordinate(const StarPoint &new_center);
};

#endif //SKYMAP_STAR_H
