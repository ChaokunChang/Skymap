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


//Abandoned
//
//class Star {
//public:
//    Star();
//    Star(int, double, double, double);
//    ~Star();
//    Star & operator=(const Star &source){
//        id = source.id;
//        spx = source.spx;
//        spy = source.spy;
//        mag = source.mag;
//        return *this;
//    }
//    int getID() { return id; }
//    double getX() { return spx; }
//    double getY() { return spy; }
//    double getMag() { return mag; }
//    //bool operator< (const Star& a) const { return a.id < id; }
//private:
//    int id;
//    double spx;
//    double spy;
//    double mag;
//};


#endif //SKYMAP_STAR_H
