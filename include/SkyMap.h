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

class ImageProperties{
public:
    int length_pixel;
    int width_pixel; //pixel based.
    int ppi; //pixels per inch.
    double focal_length; //mm

    double length_inch;
    double width_inch;
    double ppmm; //pixels per mm.
    ImageProperties(int l_p,int w_p,int ppi_,double f_l):length_pixel(l_p),width_pixel(w_p),ppi(ppi_),focal_length(f_l),
                    length_inch(l_p*1.0/ppi_),width_inch(w_p*1.0/ppi_),ppmm(1.0/inch2mm(1.0/ppi_)){}
    ImageProperties():length_pixel(0),width_pixel(0),ppi(96),focal_length(0.0),
                    length_inch(0),width_inch(0),ppmm(1.0/inch2mm(1.0/96.0)){}
    ImageProperties(double l_i, double w_i,int ppi_,double f_l): length_pixel(int(l_i*ppi_) ),width_pixel(int(w_i*ppi_)),
                    ppi(ppi_),focal_length(f_l),length_inch(l_i), width_inch(w_i),ppmm(inch2mm(ppi_)){}
    ImageProperties(const ImageProperties & property);
    void change_ppmm(double new_ppmm);
};

class Observation{
public:
    size_t count_;
    pair<double,double> range_; //range_ = {length, width}
    double scope_length;
    double scope_width;
    ImageProperties property;

    /**
     * @brief centre_
     * During simulation or evaluation, center_ stores the image's direction vector location in skymap
     * During application, center is (0,0)
     */
    StarPoint centre_;
    vector<StarPoint> stars_;
    Observation():count_(0),range_({0,0}),scope_length(0.0),scope_width(0.0){}
    void RangeStandardization();
    void setProperties(ImageProperties &prop);
    void ContentSync();
};

class GeneratedImage{
public:
    cv::Mat image_;
    string image_path_;
    vector<StarPoint> stars_; //id of skymap, location in image(based on center), sorted by x then y;
    ImageProperties propery_;
    //GeneratedImage():image_(cv::Mat::zeros(1,1,CV_8UC1)){}
    GeneratedImage(cv::Mat &image,string path,vector<StarPoint> &stars, ImageProperties &property);
};
