//
// Created by 70700 on 2019/3/3.
//

#ifndef SKYMAP_SKYMAPMATCHING_H
#define SKYMAP_SKYMAPMATCHING_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "QCSVAdapter.h"
#include "Star.h"
//#include "ImageProcessing.h"
#include "TriangleMatching.h"
#include "MyFunctions.h"

using namespace std;


class SkyMap{
public:
    int count_;
    pair<double,double> range_; //range_ = {length, width}
    StarPoint centre_;
    vector<StarPoint> stars_;
    SkyMap():count_(0),range_({0,0}){}

    vector<StarPoint> Subset(StarPoint centre, double length, double width); //generate image with given position and size;
    vector<StarPoint> Subset(StarPoint centre, double image_ratio, int num); // generate image with centre and particular number of stars.
};

class Observation{
public:
    int count_;
    pair<double,double> range_; //range_ = {length, width}
    StarPoint centre_;
    vector<StarPoint> stars_;
    Observation():count_(0),range_({0,0}){}
    void RangeStandardization();
};

class SkyMapMatching {
private:
    StarPoint __target_star; //the chosen star in image. this variable stores its location in image.
    StarPoint __matching_star; //the result given by match algorithm, this variable stores its location in SkyMap.

public:
    const int LongitudeRange = 360;
    const int LatitudeRange = 180;
    SkyMap sky_;
    Observation image_;

public:
    void LoadSky(QString &f_name);
    void LoadImage(QString &f_name);

    void GenerateSimImage(StarPoint centre, double length, double width); //generate image with given position and size;
    void GenerateSimImage(StarPoint centre, double image_ratio, int num); // generate image with centre and particular number of stars.

    void SelectTargetStar();
    void SelectTargetStar(size_t);
    int TriangleModel();
    void Match();
    int Check();

    StarPoint GetAnswer(){ return this->__matching_star;}
    StarPoint GetTargetStar(){ return this->__target_star;}

};


#endif //SKYMAP_SKYMAPMATCHING_H
