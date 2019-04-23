//
// Created by 70700 on 2019/3/3.
//

#include "TriangleMatching.h"


bool star_pair_compare(const StarPair &s1, const StarPair &s2){
    return s1.angular_distance < s2.angular_distance;
}

void TriangleMatching::LoadData(vector<StarPoint> &stars) {
    stardata_.clear();
    for(size_t i=0;i<stars.size();i++){
        StarPoint s1 = stars[i];
        for(size_t j=i+1;j<stars.size();j++){
            StarPoint s2 = stars[j];
            double dis = getSphereAD(s1.x,s1.y,s2.x,s2.y);
            if(dis < __Threshold){
                StarPair sp(s1.index, s2.index, dis);
                stardata_.push_back(sp);
            }
        }
    }
    sort(stardata_.begin(),stardata_.end(),star_pair_compare);
    __GroupNumber = 1 + stardata_[stardata_.size()-1].angular_distance / __GapWidth;
    grouphead_.resize(__GroupNumber,0);
    groupsize_.resize(__GroupNumber,0);
    grouptail_.resize(__GroupNumber,0);
    matchgroup_.resize(__GroupNumber);
    for(StarPair sp:stardata_){
        int id = sp.angular_distance / __GapWidth;
        groupsize_[id] ++;
    }
    for(int i=1;i<__GroupNumber;i++){
        grouphead_[i] = grouphead_[i - 1] + groupsize_[i - 1];
        grouptail_[i] = grouphead_[i]+groupsize_[i] ;
    }
    StatStar.resize(stars.size()+1);
}

bool explicit_pair(StarPoint &s1, StarPoint &s2){
    return getSphereAD(s1.x,s1.y,s2.x,s2.y)>=1e-6;
}

vector<StarPoint> TriangleMatching::RandomAdjacentStars(vector<StarPoint> &obv_stars, StarPoint &except) {
    int size = int(obv_stars.size());
    size_t s1 = static_cast<size_t>(random_int(0,size));
    while(!explicit_pair(except,obv_stars[s1])) s1 = static_cast<size_t>(random_int(0,size));
    size_t s2 = static_cast<size_t>(random_int(0,size));
    while((s1==s2)|| !explicit_pair(except,obv_stars[s2])) s2=static_cast<size_t>(random_int(0,size));
    return {obv_stars[s1],obv_stars[s2]};
}

void TriangleMatching::ChooseAdjacentStars(vector<StarPoint> &obv_stars, vector<StarPoint> &triangle) {
    vector<StarPoint> adjacent = RandomAdjacentStars(obv_stars, triangle[0]);

    triangle.insert(triangle.end(),adjacent.begin(),adjacent.end());

    __TargetTriangle.middle_star = triangle[0].index;
    __TargetTriangle.star1 = triangle[1].index;
    __TargetTriangle.star2 = triangle[2].index;
    qDebug()<<"The Target Triangle in image is:";
    qDebug()<<" Centre,Adjacent1,Adjacent2: "<< __TargetTriangle.middle_star <<" , "<< __TargetTriangle.star1<<
                                                " , "<< __TargetTriangle.star2 ;

}

void TriangleMatching::MatchAlgorithm(double center_edge1, double center_edge2, double edge1_edge2, double m1, double m2, double m3) {
    if(!this->__Candidate.empty()) this->__Candidate.clear();
    vector<int> Flag(__GuideStarNumber+5,0);

    size_t group1 = size_t(center_edge1 / 0.02);
    size_t group2 = size_t(center_edge2 / 0.02);
    size_t group3 = size_t(edge1_edge2 / 0.02);

//    map< int, vector<int> > StatStar;
//    map< int, vector<int> >::iterator SS_iter;
//    unordered_map<int, vector<int> > StatStar;
//    unordered_map< int, vector<int> >::iterator SS_iter;

    for (size_t i=0;i!=StatStar.size();i++) {
        StatStar[i].clear();
    }

    //扫描和标记组1中的星，把组中出现的星的flag参量标记为1，并记下与之相邻的另外一颗星。

    //使用map 数据结构存储毗邻星
    for (size_t i = grouphead_[group1]; i < grouphead_[group1] + groupsize_[group1]; i++) {
        int s1, s2;
        s1 = stardata_[i].star1;
        s2 = stardata_[i].star2;
        Flag[size_t(s1)] = 1;
        Flag[size_t(s2)] = 1;
//        SS_iter = StatStar.find(s1);
//        if (SS_iter == StatStar.end()) {
//            vector<int> v1;
//            v1.push_back(s2);
//            StatStar.insert(pair<int, vector<int> >(s1, v1));
//        }
//        else {
//            SS_iter->second.push_back(s2);
//        }

//        SS_iter=StatStar.find(s2);
//        if (SS_iter == StatStar.end()) {
//            vector<int> v2;
//            v2.push_back(s1);
//            StatStar.insert(pair<int, vector<int> >(s2, v2));
//        }
//        else {
//            SS_iter->second.push_back(s1);
//        }
        StatStar[s1].push_back(s2);
        StatStar[s2].push_back(s1);
    }
    //扫描标记组2中的星，如果该星已被标记为1，则标记为状态2；并记录与之毗邻的另一颗星；
    //（组1和组2为相同组的情况如何处理？是否需要特殊情况考虑呢？）
    size_t MatchCnt = 0;
    for (size_t i = grouphead_[group2]; i < grouphead_[group2] + groupsize_[group2]; i++) {
        int b1, b2;
        b1 = stardata_[i].star1;
        b2 = stardata_[i].star2;
        if (Flag[size_t(b1)] != 0) {
            Flag[size_t(b1)] = 2;
            for (size_t j = 0; j < StatStar[b1].size()&&MatchCnt < matchgroup_.size() ; j++) {
                matchgroup_[MatchCnt].star2 = b2; // modified
                matchgroup_[MatchCnt].star1 = StatStar[b1][j];
                matchgroup_[MatchCnt].middle_star = b1;
                MatchCnt++;
            }
        }
        if (Flag[size_t(b2)] != 0) {
            Flag[size_t(b2)] = 2;
            for (size_t j = 0; j < StatStar[b2].size()&&MatchCnt < matchgroup_.size(); j++) {
                matchgroup_[MatchCnt].star2 = b1; //modified
                matchgroup_[MatchCnt].star1 = StatStar[b2][j];
                matchgroup_[MatchCnt].middle_star = b2;
                MatchCnt++;
            }
        }
    }
    //在标记组3中寻找可以符合条件的星对组成匹配三角形。使用栈存储成功匹配的编号；
    for (size_t i = grouphead_[group3]; i < grouphead_[group3]+groupsize_[group3]; i++) {
        int c1,c2;
        c1 = stardata_[i].star1;
        c2 = stardata_[i].star2;
        for (int j = 0; j < int(MatchCnt) ; j++) {
            if ((matchgroup_[j].star1 == c1 && matchgroup_[j].star2 == c2) || (matchgroup_[j].star1 == c2 && matchgroup_[j].star2 == c1)) {
                //匹配成功，获得一组匹配三角形。标记j,或者直接输出j中的三个星的编号
                __Candidate.push_back(j);
                Flag[matchgroup_[j].middle_star] = 3;
            }
        }
    }
}

int TriangleMatching::GetCandidate(){
    int result=-1;
    if (__Candidate.empty()) {
        qDebug() << "Triangle No candidate.";
        //扩大组的范围，或换一组三角形匹配。
    }
    else {
        //从匹配成功的三角形中筛选出最终结果，需要结合星等数据选择，或者加入GPS等参考数据
        int k = __Candidate.back();
        assert(k >= 0);
        __Candidate.pop_back();
        qDebug() << "#Match(Triangle Model) succeed!";
        //cout << "The answer'id in match group(all possible answer) is: "<<result << endl;
        qDebug() <<"The matching triangle is: "<< matchgroup_[k].middle_star << " " << matchgroup_[k].star1 << " " << matchgroup_[k].star2;
        qDebug() <<"And there are still "<<__Candidate.size()<<" candidates."<<endl;

        string str="";
        for(size_t i=0; i<__Candidate.size();i++) str+= to_string(__Candidate.at(i)) +" ";
        qDebug()<<QString::fromStdString(str);

        result =  matchgroup_[static_cast<size_t>(k)].middle_star;
    }
    return result;
}
