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
#include "TriangleMatching.h"
#include "NoOptic.h"
#include "MyFunctions.h"

using namespace std;


class SkyMap{
public:
    size_t count_;
    pair<double,double> range_; //range_ = {length, width}
    StarPoint centre_;
    vector<StarPoint> stars_;
    SkyMap():count_(0),range_({0,0}){}

    vector<StarPoint> Subset(StarPoint centre, double length, double width); //generate image with given position and size;
    vector<StarPoint> Subset(StarPoint centre, double image_ratio, int num); // generate image with centre and particular number of stars.
};

class Observation{
public:
    size_t count_;
    pair<double,double> range_; //range_ = {length, width}
    StarPoint centre_;
    vector<StarPoint> stars_;
    Observation():count_(0),range_({0,0}){}
    void RangeStandardization();
};

struct Candidate{
    string model_name;
    //size_t model_type;
    StarPoint star;
    double confidence;
    Candidate():model_name("Default Model"),confidence(0.0){}
    Candidate(string name,StarPoint sp,double conf = 0.0):model_name(name),star(sp),confidence(conf){}
    //Candidate(string name,StarPoint sp):model_name(name),star(sp){}
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
    vector<Candidate> candidates_;

public:
    void LoadSky(QString &f_name);
    void LoadImage(QString &f_name);

    void GenerateSimImage(StarPoint centre, double length, double width); //generate image with given position and size;
    void GenerateSimImage(StarPoint centre, double image_ratio, int num); // generate image with centre and particular number of stars.

    void SelectTargetStar();
    void SelectTargetStar(int);
    int TriangleModel();
    int NoOpticModel();
    void Match();
    int Check();
    int CheckAllCandidates();

    StarPoint GetAnswer(){ return this->__matching_star;}
    StarPoint GetTargetStar(){ return this->__target_star;}

};


#endif //SKYMAP_SKYMAPMATCHING_H
