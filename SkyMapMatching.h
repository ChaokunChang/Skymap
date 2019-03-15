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
#include "CSVAdapter.h"
#include "Star.h"
#include "TriangleMatching.h"
#include "MyFunctions.h"
//#include "ThirdParty/MatPlot/64bit/MatPlot.h"

using namespace std;


class SkyMap{
public:
    int number_;
    pair<int,int> size_; //size_ = {length, width}
    StarPoint centre_;
    vector<StarPoint> stars_;
    SkyMap():number_(0),size_({0,0}){};
};

class Observation{
public:
    int number_;
    pair<int,int> size_; //size_ = {length, width}
    StarPoint centre_;
    vector<StarPoint> stars_;
    Observation():number_(0),size_({0,0}){};
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
    void LoadSky(string &f_name);
    void LoadImage(string &f_name);

    vector<StarPoint> Subset(float x_s, float x_len, float y_s, float y_len); //In fact, this method should be write in Class SkyMap.
    void GenerateSimImage(StarPoint centre, float length, float width); //generate image with given position and size;
    void GenerateSimImage(StarPoint centre, float image_ratio, int num); // generate image with centre and particular number of stars.

    void SelectTargetStar();
    int TriangleModel();
    void Match();
    bool Check();

    StarPoint GetAnswer(){ return this->__matching_star;};
    StarPoint GetTargetStar(){ return this->__target_star;};

};


#endif //SKYMAP_SKYMAPMATCHING_H
