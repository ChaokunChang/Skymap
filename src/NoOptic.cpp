//
// Created by 70700 on 2019/3/3.
//

#include "NoOptic.h"

NoOptic::NoOptic(vector<StarPoint> &sky, vector<StarPoint> &obv) {
    for(int i=0;i<sky.size();i++){
        this->SkyStars.push_back({sky[i].x,sky[i].y});
    }
    for(int i=0;i<obv.size();i++){
        this->ImageStars.push_back({obv[i].x,obv[i].y});
    }
    FinalResult = -1;
}

int NoOptic::ExeNoOptic(){
    //第一步，处理导航星表SkyStars;
    for(size_t i=0; i<SkyStars.size(); i++){
        int main_star = int(i);
        vector<bool> eigen_vector;
        eigen_vector = GetEigenVector(main_star);

        EigenVecStruct e;
        e.eigen_vec = eigen_vector;
        e.star_index = main_star;
        StarEigens.push_back(e);
    }
    int ms = GetMidStar();
    if(Match(ms)) {
        cout<<"Find the appropriate star:"<<FinalResult<<endl;
        return FinalResult;
    }
    else cout<<"No appropriate star. Try another star to check."<<endl;
    return -1;
}

bool check_dis(double dis){
    if(dis < SightSize && dis > 1) return true;
    return false;
}

bool is_above_line(double x1,double y1,double x2,double y2,double x3,double y3){
    double k = (y1-y2)/(x1-x2);
    double kt = (y3-y1)/(x3-x1);
    return k>=kt;
}

int star_partition(double x, double y){
    int b1 = x*35/SightSize;
    int b2 = (y+180)*80/360;
    int par = b1*80+b2;
    if(par>PartitionNumber-1) return PartitionNumber-1;
    else return par;
}

vector<bool> NoOptic::GetEigenVector(int StarNum){
    int x;
    if(StarNum == 1310) {
        x=0;
    }
    bool eigen_array[PartitionNumber]={false};
    vector<bool> eigen_vec;
    vector<int> AdjacentStars;
    vector<double> AdjacentDistances;
    vector<pair<double,double>> PolarCoordinates;
    double ref_dis=100000;
    int ref_dis_id=0;

    //确定邻星
    for(int i=0;i<SkyStars.size(); i++){
        if(i==StarNum) continue;
        double dis = cal_dis(SkyStars[StarNum].first,SkyStars[StarNum].second,SkyStars[i].first,SkyStars[i].second);
        if(check_dis(dis)){
            //将符合条件的邻星的编号存储到vector中，并记录最近的那颗星。
            if(dis<ref_dis && dis>R0) {
                ref_dis = dis;
                ref_dis_id = i;
            }
            AdjacentStars.push_back(i);
            AdjacentDistances.push_back(dis);
        }

    }

    //建立极坐标系
    for(int i=0; i<AdjacentStars.size(); i++){
        int id = AdjacentStars[i];
        double r,zeta,s;
        r = AdjacentDistances[i] / ref_dis;
        s = cal_dis(SkyStars[id].first,SkyStars[id].second,SkyStars[ref_dis_id].first,SkyStars[ref_dis_id].second)/ref_dis;
        double tmp = (1+r*r-s*s)/(2*r);
        if(tmp>1){

            if(tmp>1.001) cout<<"error zeta!"<<endl;
            else tmp = 1;
        }
        if(tmp<-1){
            if(tmp<-1.001) cout<<"error zeta!"<<endl;
            else tmp = -1;
        }
        zeta = acos(tmp)*180/Pi;
        if(!(zeta <180 && zeta>-180)){
            if(zeta < -180 && zeta>-180.001) zeta = -180;
            else if(zeta > 180 && zeta<1980.001) zeta = 180;
            else x = -1;
        }
        if(zeta < 0) zeta+=180;
        if(!is_above_line(SkyStars[StarNum].first,SkyStars[StarNum].second,SkyStars[ref_dis_id].first,SkyStars[ref_dis_id].second,SkyStars[i].first,SkyStars[i].second)) zeta = -zeta;
        PolarCoordinates.push_back(pair<double,double>(r,zeta));
    }

    //判断vector中的邻星属于M*N的哪一个区域，并做相应的计数操作。
    for(int i=0; i<AdjacentStars.size(); i++){
        int id = AdjacentStars[i];
        double r,zeta;
        r = PolarCoordinates[i].first;
        zeta = PolarCoordinates[i].second;
        int pr = star_partition(r,zeta);

        //创建eigenvector
        eigen_array[pr] = true;

        //建立星模式表；
        if(PartitionCounter[pr].star_array.empty()) PartitionCounter[pr].number = 0;
        PartitionCounter[pr].star_array.push_back(StarNum);
        PartitionCounter[pr].number += 1;
    }

    for(int i=0; i<PartitionNumber; i++){
        if(eigen_array[i]) eigen_vec.push_back(true);
        else eigen_vec.push_back(false);
    }
    return eigen_vec;

}

int NoOptic::GetMidStar(){
    double x0=0.0,y0=0.0;
    double min_dis=1000000;
    int index;
    for(int i=0; i<ImageStars.size(); i++){
        double dis = cal_dis(x0,y0,ImageStars[i].first,ImageStars[i].second);
        if(dis<min_dis) index = i;
    }
    return index;
}

bool Check(int id){
    /*Ready to be completed.*/
    return true;
}

bool NoOptic::Match(int main_star){
    bool eigen_array[PartitionNumber]={false};
    vector<bool> eigen_vec;
    vector<int> AdjacentStars;
    vector<double> AdjacentDistances;
    vector<pair<double,double>> PolarCoordinates;

    int StarCounter[StarNumber]={0};
    double ref_dis=100000;
    int ref_dis_id=0;

    for(int i=0;i<ImageStars.size(); i++){
        if(i==main_star) continue;
        double dis = cal_dis(ImageStars[main_star].first,ImageStars[main_star].second,ImageStars[i].first,ImageStars[i].second);
        if(check_dis(dis)){
            //将符合条件的邻星的编号存储到vector中，并记录最近的那颗星。
            if(dis<ref_dis && dis>R0) {
                ref_dis = dis;
                ref_dis_id = i;
            }
            AdjacentStars.push_back(i);
            AdjacentDistances.push_back(dis);
        }
    }

    //建立极坐标系
    for(int i=0; i<AdjacentStars.size(); i++){
        int x;
        int id = AdjacentStars[i];
        double r,zeta,s;
        r = AdjacentDistances[i] / ref_dis;
        s = cal_dis(ImageStars[id].first,ImageStars[id].second,ImageStars[ref_dis_id].first,ImageStars[ref_dis_id].second)/ref_dis;
        double tmp = (1+r*r-s*s)/(2*r);
        if(tmp>1){

            if(tmp>1.001) cout<<"error zeta!"<<endl;
            else tmp = 1;
        }
        if(tmp<-1){
            if(tmp<-1.001) cout<<"error zeta!"<<endl;
            else tmp = -1;
        }
        zeta = acos(tmp)*180/Pi;
        if(!(zeta <180 && zeta>-180)){
            if(zeta < -180 && zeta>-180.001) zeta = -180;
            else if(zeta > 180 && zeta<1980.001) zeta = 180;
            else x = -1;
        }
        if(zeta < 0) zeta+=180;
        if(!is_above_line(ImageStars[main_star].first,ImageStars[main_star].second,ImageStars[ref_dis_id].first,ImageStars[ref_dis_id].second,ImageStars[i].first,ImageStars[i].second)) zeta = -zeta;
        PolarCoordinates.push_back(pair<double,double>(r,zeta));
    }

    //判断vector中的邻星属于M*N的哪一个区域，并做相应的计数操作。
    for(int i=0; i<AdjacentStars.size(); i++){
        int id = AdjacentStars[i];
        double r,zeta;
        r = PolarCoordinates[i].first;
        zeta = PolarCoordinates[i].second;
        int pr = star_partition(r,zeta);

        //创建eigenvector
        eigen_array[pr] = true;

        //处理计数器
        for(int j=0; j<PartitionCounter[i].number; j++){
            StarCounter[PartitionCounter[i].star_array[j]] += 1;
        }

    }

    //找出计数器的最大值
    int max_counter=0;
    int max_num=0;
    for(int i=0; i<StarNumber; i++){
        if(StarCounter[i]>max_num){
            max_num=StarCounter[i];
            max_counter = i;
        }
    }

    //结果可能不止一个，所以用一个vecotr存放初步结果
    vector<int> preliminary_results;
    for(int i=0; i<StarNumber; i++){
        if(StarCounter[i]==max_num) preliminary_results.push_back(i);
    }

    //进一步的检查
    for(int i=0; i<preliminary_results.size(); i++){
        if(Check(preliminary_results[i])) {
            FinalResult = preliminary_results[i];
            return true;
        }
    }
    return false;
}
