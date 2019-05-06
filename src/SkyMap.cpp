#include "SkyMap.h"



vector<StarPoint> SkyMap::Subset(const StarPoint &centre, double length, double width) {
    //Some Check here.
    vector<StarPoint> stars;
    for(StarPoint point : stars_){
        if(point.InRange(centre,length,width)){
            stars.push_back(point);
        }
    }
    return stars;
}

vector<StarPoint> SkyMap::Subset(const StarPoint &centre, double image_ratio, int numi=0) {
    double upper_bound = 20;
    double lower_bound = 1;
    double length = (upper_bound+lower_bound)/2;
    vector<StarPoint> stars;
    double width;
    size_t num = size_t(numi);
    while( abs(lower_bound-upper_bound)>0.01 ){
        width = length/image_ratio;
        stars = this->Subset(centre,length,width);
        if(stars.size() == num) break;
        else if(stars.size() < num){
            stars.clear();
            lower_bound = length;
            length = (upper_bound+lower_bound)/2;
        } else{
            stars.clear();
            upper_bound = length;
            length = (upper_bound+lower_bound)/2;
        }
    }
    if(stars.size() == num) {
        qDebug()<<"The number of star of this Subset:"<<stars.size();
    } else{
        qDebug()<<"Invalid subset! You can try another ratio or center.";
        stars.clear();
    }
    return stars;
}

void Observation::RangeStandardization(){
    if(this->count_==0){
        this->range_ = {0,0};
        return;
    }else{
        double lx = 10000.0,rx = -10000.0,ly = 10000.0,ry = -10000.0;
        for(size_t i=0;i<stars_.size();i++){
            lx = min(stars_[i].x,lx);
            rx = max(stars_[i].x,rx);
            ly = min(stars_[i].x,ly);
            ry = max(stars_[i].x,ry);
        }
        this->range_ = {rx-lx,ry-ly};
    }
}

void Observation::ContentSync(){
    this->count_ = this->stars_.size();
}

void ImageProperties::change_ppmm(double new_ppmm){
    ppmm = new_ppmm;
    ppi = static_cast<int>(mm2inch(ppmm));
    length_inch = length_pixel / mm2inch(ppmm);
    width_inch = width_pixel / mm2inch(ppmm);
}

ImageProperties::ImageProperties(const ImageProperties& p){
    this->focal_length = p.focal_length;
    this->length_pixel = p.length_pixel;
    this->width_pixel = p.width_pixel;
    this->change_ppmm(p.ppmm);
    this->ppi = p.ppi;
}

GeneratedImage::GeneratedImage(cv::Mat &image,string path, vector<StarPoint> &stars, ImageProperties &property){
    this->image_ = image;
    this->stars_ = stars;
    this->propery_ = property;
    this->image_path_ = path;
}
