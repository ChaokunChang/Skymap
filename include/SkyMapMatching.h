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
#include <sstream>
#include "QCSVAdapter.h"
#include "Star.h"
#include "TriangleMatching.h"
#include "NoOptic.h"
#include "MyFunctions.h"

using namespace std;
struct image_properties{
    int imageWidth;
    int imageHeight; //pixel based.
    double imageWidthL;
    double imageHeightL;
    double focal_length;
    image_properties(int iw,int ih,double iwl,double ihl,double fl):imageWidth(iw),imageHeight(ih),
        imageWidthL(iwl),imageHeightL(ihl),focal_length(fl){}
    image_properties():imageWidth(0),imageHeight(0),
        imageWidthL(0.0),imageHeightL(0.0),focal_length(0.0){}
};

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
    int imageWidth,imageHeight; //pixel based
    double imageWidthL,imageHeightL;
    double focal_length;
    StarPoint centre_;
    vector<StarPoint> stars_;
    Observation():count_(0),range_({0,0}){}
    void RangeStandardization();
    void setProperties(image_properties prop);
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

class ModelEvaluation{
public:
    string model_name;
    string property;
    int round;
    double correctness;
};

class MissingEvaluation: public ModelEvaluation{
public:
    double missing_rate;
    int missing_num;
};

class DeviationEvaluation: public ModelEvaluation{
public:
    double deviation_rate;
};

class RedundanceEvaluation: public ModelEvaluation{
public:
    double redundance_rate;
    int redundance_num;
};

template <class T>
class NoiseEvaluation{
    //Noise include missing noise,deviation noise,and redundance noise.
public:
    int total_round;
    vector<T> tests;
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
    void LoadImage(QString &f_name,image_properties);

    void GenerateSimImage(StarPoint centre, double length, double width); //generate image with given position and size;
    void GenerateSimImage(StarPoint centre, double image_ratio, int num); // generate image with centre and particular number of stars.

    void SelectTargetStar();
    void SelectTargetStar(int);
    int TriangleModel();
    int NoOpticModel();
    void Match(size_t model=0);
    int Check();
    int CheckAllCandidates();
    //void initPara(int,int,double,double,double);
    StarPoint GetAnswer(){ return this->__matching_star;}
    StarPoint GetTargetStar(){ return this->__target_star;}
    Evaluation ExeSimulation(size_t model);

};


#endif //SKYMAP_SKYMAPMATCHING_H
