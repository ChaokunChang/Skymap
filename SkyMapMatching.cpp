//
// Created by 70700 on 2019/3/3.
//

#include "SkyMapMatching.h"
#include "extraction.h"
#include <fstream>

using namespace cv;

void SkyMapMatching::LoadSky(string &f_name) {
    CSVAdapter csv_sky(f_name);
    Star s;
    //int count = 0;
    while(csv_sky.hasRecord()){
        s = csv_sky.getNextRecord();
        StarPoint sp(s.getID(),s.getX(),s.getY(),s.getMag());
        this->sky_.stars_.push_back(sp);
        this->sky_.number_++;
    }
    this->sky_.size_ = {360,180};
    this->sky_.centre_ = StarPoint(-1,180.0,0,0);
}

void SkyMapMatching::LoadImage_img(string &f_name, string &output_file) {
    Mat srcimg, segimg;
    vector<pair<double, double>> centroids;

    // read srcimg
    cout << "Reading image..." << endl;
    srcimg = read_img(f_name);

    // segmentation
    cout << "Preprocessing image..." << endl;
    segimg = preprocess_img(srcimg);

    // centroids
    cout << "Get centroids of stars..." << endl;
    centroids = get_centroids(segimg);

    fstream fout;

    fout.open(output_file, ios::out);
    int length = centroids.size();
    for (int i = 0; i < length; i++)
        fout << i << ',' << centroids[i].first << ',' << centroids[i].second << ',' << 0 << endl;
    fout.close();
}

void SkyMapMatching::LoadImage(string &f_name) {
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

void SkyMapMatching::SelectTargetStar() {
    StarPoint cen = this->image_.centre_;
    float mindis = INT32_MAX;
    int target = 0;
    for(int i=0;i<this->image_.stars_.size();i++){
        StarPoint s = this->image_.stars_[i];
        float dis = s.Distance(cen);
        if(dis<mindis){
            mindis = dis;
            target = i;
        }
    }
    this->__target_star = this->image_.stars_[target];
}


int SkyMapMatching::TriangleModel() {
    TriangleMatching TM(sky_.stars_.size(), 12.0, 0.02);
    TM.LoadData(sky_.stars_);

    vector<StarPoint> triangle;
    triangle.push_back(this->__target_star);

    float dis12,dis13,dis23;
    while(true){
        TM.ChooseAdjacentStars(image_.stars_,triangle);
        dis12 = cal_dis(triangle[0].x,triangle[0].y,triangle[1].x,triangle[1].y);
        dis13 = cal_dis(triangle[0].x,triangle[0].y,triangle[2].x,triangle[2].y);
        dis23 = cal_dis(triangle[1].x,triangle[1].y,triangle[2].x,triangle[2].y);
        if(dis12 < TM.GetThreshold() && dis13 < TM.GetThreshold() && dis23 < TM.GetThreshold()) break;
        triangle.pop_back();
        triangle.pop_back();
    }

    int result_index = TM.MatchAlgorithm(dis12,dis13,dis23,triangle[0].magnitude,triangle[1].magnitude,triangle[2].magnitude);

    cout<<"Match Ended!"<<endl;
    return result_index;
}

void SkyMapMatching::Match() {
    int skymap_index = TriangleModel();
    this->__matching_star = this->sky_.stars_[skymap_index];
}


bool SkyMapMatching::Check() {
    if(this->__matching_star.index == this->__target_star.index) return true;
    else return false;
}

vector<StarPoint> SkyMapMatching::Subset(float x_s, float x_len, float y_s, float y_len) {
    vector<StarPoint> stars;
    for(StarPoint point : sky_.stars_){
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
        this->image_.size_ = {length<0?0:length , width<0?0:width};
    }
    vector<StarPoint> stars = Subset(centre.x-(length/2),length, centre.y-(width/2),width);
    if(!this->image_.stars_.empty()) this->image_.stars_.clear();

    for(int i=0;i<stars.size();i++){
        stars[i].x -= centre.x-(length/2);
        stars[i].y -= centre.y-(width/2);
    }
    this->image_.size_ = {length , width};
    this->image_.stars_.insert(this->image_.stars_.end(),stars.begin(),stars.end());
    this->image_.number_ = this->image_.stars_.size();
    this->image_.centre_ = centre;

    //double *longitude = new double[stars.size()];
    //double *latitude = new double[stars.size()];
    cout<<"The Generated image is:"<<endl;
    for(int i=0;i<stars.size();i++){
        cout<<stars[i].index<<": ("<<stars[i].x<<" , "<<stars[i].y<<")"<<endl;
        //longitude[i] = (stars[i].x);
        //latitude[i] = (stars[i].y);
    }
    //MatPlot::scatter(longitude,latitude,stars.size());
    cout<<"The number of stars is:"<<stars.size()<<endl;
}

void SkyMapMatching::GenerateSimImage(StarPoint centre, float image_ratio, int num) {
    float upper_bound = 15;
    float lower_bound = 1;
    float length = (upper_bound+lower_bound)/2;
    vector<StarPoint> stars;
    float width;
    float x0,y0;
    while( abs(lower_bound-upper_bound)<0.01 ){
        width = length/image_ratio;
        x0 = centre.x-(length/2);
        y0 = centre.y-(width/2);
        stars = Subset(x0,length, y0,width);
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

    if(!this->image_.stars_.empty()) this->image_.stars_.clear();
    if(stars.size() == num) {
        this->image_.size_ = {length , width};
        for(int i=0;i<stars.size();i++){
            stars[i].x -= x0;
            stars[i].y -= y0;
        }
        this->image_.stars_.insert(this->image_.stars_.end(),stars.begin(),stars.end());
        this->image_.number_ = this->image_.stars_.size();
        this->image_.centre_ = centre;
    } else{
        this->image_.number_ = 0;
        this->image_.size_ = {length , width};
        this->image_.centre_ = centre;
        cout<<"Can Not get such set! You can try another ratio or center."<<endl;
    }
}
