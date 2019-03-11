//
// Created by 70700 on 2019/3/3.
//

#include "SkyMapMatching.h"
//#include "ThirdParty/opencv-4.0.1/include/opencv2/opencv.hpp"


void SkyMapMatching::LoadSky(string f_name) {
    CSVAdapter csv_sky(f_name);
    Star s;
    //int count = 0;
    while(csv_sky.hasRecord()){
        s = csv_sky.getNextRecord();
        StarPoint sp(s.getID(),s.getX(),s.getY(),s.getMag());
        this->sky_.stars_.push_back(sp);
        this->sky_.number_++;
    }
}

void SkyMapMatching::LoadImage(string f_name) {
    //Not completed now.
    CSVAdapter csv_sky(f_name);
    Star s;
    //int count = 0;
    while(csv_sky.hasRecord()){
        s = csv_sky.getNextRecord();
        StarPoint sp(s.getID(),s.getX(),s.getY(),s.getMag());
        this->image_.stars_.push_back(sp);
        this->image_.number_++;
    }
}

void SkyMapMatching::TriangleModel() {
    TriangleMatching TM(sky_.stars_.size(), 12.0, 0.02);
    TM.LoadData(sky_.stars_);
    float dis12,dis13,dis23;
    vector<StarPoint> triangle;
    while(1){
        triangle = TM.ChooseTargetStars(image_.stars_);
        dis12 = cal_dis(triangle[0].x,triangle[0].y,triangle[1].x,triangle[1].y);
        dis13 = cal_dis(triangle[0].x,triangle[0].y,triangle[2].x,triangle[2].y);
        dis23 = cal_dis(triangle[1].x,triangle[1].y,triangle[2].x,triangle[2].y);
        if(dis12 < TM.GetThreshold() && dis13 < TM.GetThreshold() && dis23 < TM.GetThreshold()) break;
    }

    TM.MatchAlgorithm(dis12,dis13,dis23,triangle[0].magnitude,triangle[1].magnitude,triangle[2].magnitude);
    cout<<"Match Ended!"<<endl;
}

void SkyMapMatching::Match() {
    TriangleModel();
}


void SkyMapMatching::Check() {

}

void SkyMapMatching::GetAnswer() {

}

vector<StarPoint> SkyMapMatching::Subset(float x_s, float x_len, float y_s, float y_len) {
    vector<StarPoint> stars;
    for(int i=0;i<sky_.stars_.size();i++){
        StarPoint point = sky_.stars_[i];
        if(between(point.x,x_s,x_s+x_len) && between(point.y, y_s, y_s+y_len)){
            stars.push_back(point);
        }
    }
    return stars;
}

void SkyMapMatching::GenerateSimImage(StarPoint centre, float length, float width) {
    //Some Check here.
    if(length<=0 || width<=0 ){
        if(!this->image_.stars_.empty()) this->image_.stars_.clear();
        this->image_.number_ = 0;
        this->image_.size_ = length * width;
    }
    vector<StarPoint> stars = Subset(centre.x-(length/2),length, centre.y-(width/2),width);
    if(!this->image_.stars_.empty()) this->image_.stars_.clear();

    this->image_.size_ = length * width;
    this->image_.stars_.insert(this->image_.stars_.end(),stars.begin(),stars.end());
    this->image_.number_ = this->image_.stars_.size();

    double *longitude = new double[stars.size()];
    double *latitude = new double[stars.size()];
    cout<<"The Generated image is:"<<endl;
    for(int i=0;i<stars.size();i++){
        cout<<stars[i].index<<": ("<<stars[i].x<<" , "<<stars[i].y<<")"<<endl;
        longitude[i] = (stars[i].x);
        latitude[i] = (stars[i].y);
    }
    //MatPlot::scatter(longitude,latitude,stars.size());
    cout<<"The number of stars is:"<<stars.size()<<endl;
}

void SkyMapMatching::GenerateSimImage(StarPoint centre, float image_ratio, int num) {
    float upper_bound = 15;
    float lower_bound = 1;
    float length = (upper_bound+lower_bound)/2;
    vector<StarPoint> stars;
    int width;
    while( abs(lower_bound-upper_bound)<0.01 ){
        width = length/image_ratio;
        stars = Subset(centre.x-(length/2),length, centre.y-(width/2),width);
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
        if(!this->image_.stars_.empty()) this->image_.stars_.clear();
        this->image_.size_ = length * width;
        this->image_.stars_.insert(this->image_.stars_.end(),stars.begin(),stars.end());
        this->image_.number_ = this->image_.stars_.size();
    } else{
        if(!this->image_.stars_.empty()) this->image_.stars_.clear();
        this->image_.number_ = 0;
        this->image_.size_ = length * width;
        cout<<"Can Not get such set! You can try another ratio or center."<<endl;
    }
}
