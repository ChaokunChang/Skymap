//
// Created by 70700 on 2019/3/3.
//

#include "NoOptic.h"

NoOptic::NoOptic(){
    //this->SkyStars = NULL;

}

NoOptic::NoOptic(vector<StarPoint> &sky,NoOpticPara para){
    LowerAdjacent = para.LowerAdjacent;
    UpperLimit = para.UpperLimit;
    LowerLimit = para.LowerLimit;
    RadialPartition = para.RadialPartition;
    CirclePartition = para.CirclePartition;
    PartitionNumber = para.PartitionNumber; //35 * 80 (radial * circular)

    StarNumber = sky.size()+5;

    PartitionCounter.resize(PartitionNumber);

    CandidateNum = 0;

    for(size_t main_star=0; main_star<sky.size(); main_star++){
        vector<bool> eigen_vector;
        eigen_vector = GetEigenVector(main_star,sky);

        EigenVecStruct e;
        e.eigen_vec = eigen_vector;
        e.star_index = main_star;
        StarEigens.push_back(e);

        //建立星模式表；
        for(size_t i=0;i<eigen_vector.size();i++){
            if(eigen_vector[i]){
                if(PartitionCounter[i].star_array.empty()) PartitionCounter[i].number = 0;
                PartitionCounter[i].star_array.push_back(main_star);
                PartitionCounter[i].number += 1;
            }
        }

    }

}

NoOptic::NoOptic(vector<StarPoint> &sky) {
    LowerAdjacent = 2.0;
    UpperLimit = 15.0;
    LowerLimit = 1.0;
    RadialPartition = 35;
    CirclePartition = 80;
    PartitionNumber = 2800; //35 * 80 (radial * circular)
    StarNumber = sky.size()+5;

    PartitionCounter.resize(PartitionNumber);

    CandidateNum = 0;

    for(size_t main_star=0; main_star<sky.size(); main_star++){
        vector<bool> eigen_vector;
        eigen_vector = GetEigenVector(main_star,sky);

        EigenVecStruct e;
        e.eigen_vec = eigen_vector;
        e.star_index = main_star;
        StarEigens.push_back(e);

        //建立星模式表；
        for(size_t i=0;i<eigen_vector.size();i++){
            if(eigen_vector[i]){
                if(PartitionCounter[i].star_array.empty()) PartitionCounter[i].number = 0;
                PartitionCounter[i].star_array.push_back(main_star);
                PartitionCounter[i].number += 1;
            }
        }

    }
}

int NoOptic::GetCandidate(){
    if(__Candidate.empty()) return -1;
    int result = static_cast<int>(this->__Candidate.back());
    __Candidate.pop_back();
    CandidateNum--;
    return result;
}

size_t NoOptic::ExeNoOptic(size_t target,vector<StarPoint> &ImageStars){
    //load image? -- do not need.
    return Match(target,ImageStars);
}

bool is_above_line(double x1,double y1,double x2,double y2,double x3,double y3){
    double k = (y1-y2)/(x1-x2);
    double kt = (y3-y1)/(x3-x1);
    return k>=kt;
}

size_t NoOptic::star_partition(double x, double y){
    int b1 = x*RadialPartition/UpperLimit;
    int b2 = (y+180)*CirclePartition/360;
    int par = b1*CirclePartition+b2;
    if(par>PartitionNumber-1) return PartitionNumber-1;
    else return par;
}

vector<bool> NoOptic::GetEigenVector(size_t StarNum, vector<StarPoint> &SkyStars){
    vector<bool> eigen_vec(PartitionNumber,false);
    vector<size_t> AdjacentStars;
    vector<double> AdjacentDistances;
    vector<pair<double,double>> PolarCoordinates;
    double ref_dis=100000;
    size_t ref_dis_id=0;

    //确定邻星
    for(size_t i=0;i<SkyStars.size(); i++){
        if(i==StarNum) continue;
        double dis = getSphereAD(SkyStars[StarNum].x,SkyStars[StarNum].y,SkyStars[i].x,SkyStars[i].y);
        if(dis < UpperLimit && dis > LowerLimit){
            //将符合条件的邻星的编号存储到vector中，并记录最近的那颗星。
            if(dis<ref_dis && dis>LowerAdjacent) {
                ref_dis = dis;
                ref_dis_id = i;
            }
            AdjacentStars.push_back(i);
            AdjacentDistances.push_back(dis);
        }

    }

    //建立极坐标系
    for(size_t i=0; i<AdjacentStars.size(); i++){
        size_t id = AdjacentStars[i];
        double r,zeta,s;
        r = AdjacentDistances[i] / ref_dis;
        s = getSphereAD(SkyStars[id].x,SkyStars[id].y,SkyStars[ref_dis_id].x,SkyStars[ref_dis_id].y)/ref_dis;
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
            else if(zeta > 180 && zeta<180.001) zeta = 180;
        }
        if(zeta < 0) zeta+=180;
        if(!is_above_line(SkyStars[StarNum].x,SkyStars[StarNum].y,SkyStars[ref_dis_id].x,SkyStars[ref_dis_id].y,SkyStars[i].x,SkyStars[i].y)) zeta = -zeta;
        PolarCoordinates.push_back(pair<double,double>(r,zeta));
    }
    //判断vector中的邻星属于M*N的哪一个区域，并做相应的计数操作。
    for(size_t i=0; i<PolarCoordinates.size(); i++){
        double r,zeta;
        r = PolarCoordinates[i].first;
        zeta = PolarCoordinates[i].second;
        size_t pr = star_partition(r,zeta);
        //创建eigen vector
        eigen_vec[pr] = true;
    }
    return eigen_vec;
}

size_t NoOptic::Match(size_t main_star,vector<StarPoint> &ImageStars){
    vector<bool> eigen_vec;
    vector<size_t> AdjacentStars;
    vector<double> AdjacentDistances;
    vector<pair<double,double>> PolarCoordinates;

    vector<size_t> StarCounter(StarNumber,0);

    vector<bool> image_e = GetEigenVector(main_star, ImageStars);
    for(size_t i=0;i<image_e.size();i++){
        if(image_e[i]){
            for(size_t j=0;j<PartitionCounter[i].number;j++){
                StarCounter[PartitionCounter[i].star_array[j]] += 1;
            }
        }
    }
    //找出计数器的最大值
    size_t max_counter=0;
    size_t max_num=0;
    for(size_t i=0; i<StarNumber; i++){
        if(StarCounter[i]>max_num){
            max_num=StarCounter[i];
            max_counter = i;
        }
    }
    //结果可能不止一个，所以用一个vecotr存放初步结果
    this->__Candidate.clear();
    for(size_t i=0; i<StarNumber; i++){
        if(StarCounter[i]==max_num) this->__Candidate.push_back(i);
    }
    CandidateNum = __Candidate.size();
    return __Candidate.size();
}
