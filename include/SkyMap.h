#include <iostream>
#include "ImageProcessing.h"
#include <malloc.h>
#include <algorithm>
using namespace std;

class SkyMap{
public:
    size_t count_;
    pair<double,double> range_; //range_ = {length, width}
    StarPoint centre_;
    vector<StarPoint> stars_;
    SkyMap():count_(0),range_({0,0}){}

    vector<StarPoint> Subset(const StarPoint &centre, double length, double width); //generate image with given position and size;
    vector<StarPoint> Subset(const StarPoint &centre, double image_ratio, int num); // generate image with centre and particular number of stars.
};

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

class Observation{
public:
    size_t count_;
    pair<double,double> range_; //range_ = {length, width}
    int imageWidth,imageHeight; //pixel based
    double imageWidthL,imageHeightL;
    double focal_length;
    StarPoint centre_;
    vector<StarPoint> stars_;
    Observation():count_(0),range_({0,0}),imageWidth(0),imageHeight(0),imageWidthL(0),imageHeightL(0),focal_length(0){}
    void RangeStandardization();
    void setProperties(image_properties &prop);
    void ContentSync();
};
