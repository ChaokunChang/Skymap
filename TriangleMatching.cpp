//
// Created by 70700 on 2019/3/3.
//

#include "TriangleMatching.h"


bool star_pair_compare(const StarPair &s1, const StarPair &s2){
    return s1.angular_distance < s2.angular_distance;
}

void TriangleMatching::LoadData(vector<StarPoint> &stars) {

    for(int i=0;i<stars.size();i++){
        StarPoint s1 = stars[i];
        for(int j=i+1;j<stars.size();j++){
            StarPoint s2 = stars[j];
            float dis = cal_dis(s1.x,s1.y,s2.x,s2.y);
            if(dis < Threshold){
                StarPair sp(s1.index, s2.index, dis);
                stardata_.push_back(sp);
            }
        }
    }
    sort(stardata_.begin(),stardata_.end(),star_pair_compare);
    GroupNumber = stardata_[stardata_.size()-1].angular_distance / GapWidth;
    grouphead_.resize(GroupNumber,0);
    groupsize_.resize(GroupNumber,0);
    grouptail_.resize(GroupNumber,0);
    matchgroup_.resize(GroupNumber);
    for(StarPair sp:stardata_){
        int id = sp.angular_distance / GapWidth;
        groupsize_[id] ++;
    }
    for(int i=1;i<GroupNumber;i++){
        grouphead_[i] = grouphead_[i - 1] + groupsize_[i - 1];
        grouptail_[i] = grouphead_[i]+groupsize_[i] ;
    }

}

vector<StarPoint> TriangleMatching::Random3Stars(vector<StarPoint> &obv_stars) {
    int size = obv_stars.size();
    srand(time(NULL));
    int s1 = rand()%size;
    int s2 = rand()%size;
    while(s2 == s1) s2=rand()%size;
    int s3 = rand()%size;
    while(s3 == s2 || s3 == s1) s3=rand()%size;

/*
    vector< pair<int,pair<int,int> > > vec;
    for(int i=0;i<size;i++){
        StarPoint obs1 = obv_stars[i];
        for(int j=i+1;j<size;j++){
            StarPoint obs2 = obv_stars[j];
            float dis12 = cal_dis(obs1.x,obs1.y,obs2.x,obs2.y);
            for(int k=j+1;k<size;k++){
                StarPoint obs3 = obv_stars[k];
                float dis13 = cal_dis(obs1.x,obs1.y,obs3.x,obs3.y);
                float dis23 = cal_dis(obs2.x,obs2.y,obs3.x,obs3.y);
                if(dis12 < Threshold && dis13 < Threshold && dis23 < Threshold){
                    vec.push_back({i,{j,k}});
                }
            }

        }
    }
    for(int i=0;i<vec.size();i++){
        cout<<vec[i].first<<" -- "<<vec[i].second.first<<" -- "<<vec[i].second.second<<endl;
    }
*/

    ChosedTriangle.middle_star = obv_stars[s1].index;
    ChosedTriangle.star1 = obv_stars[s2].index;
    ChosedTriangle.star2 = obv_stars[s3].index;

    cout<<"The chosen stars in Random method are:"<<endl;
    cout<<"star'id in image:"<<ChosedTriangle.middle_star<<" "<<ChosedTriangle.star1<<" "<<ChosedTriangle.star2<<endl;

    return {obv_stars[s1],obv_stars[s2],obv_stars[s3]};
}

vector<StarPoint> TriangleMatching::ChooseTargetStars(vector<StarPoint> &obv_stars) {
    return Random3Stars(obv_stars);
}

void TriangleMatching::MatchAlgorithm(float ad12, float ad23, float ad13, float m1, float m2, float m3) {
    vector<int> Flag(GuideStarNumber,0);

    int group1 = ad12 / 0.02;
    int group2 = ad23 / 0.02;
    int group3 = ad13 / 0.02;

    map< int, vector<int> > StatStar;
    map< int, vector<int> >::iterator SS_iter;

    //扫描和标记组1中的星，把组中出现的星的flag参量标记为1，并记下与之相邻的另外一颗星。

    //使用map 数据结构存储毗邻星
    for (int i = grouphead_[group1]; i < grouphead_[group1] + groupsize_[group1]; i++) {
        int s1, s2;
        s1 = stardata_[i].star1;
        s2 = stardata_[i].star2;
        Flag[s1] = 1;
        Flag[s2] = 1;
        SS_iter = StatStar.find(s1);
        if (SS_iter == StatStar.end()) {
            vector<int> v1;
            v1.push_back(s2);
            StatStar.insert(pair<int, vector<int> >(s1, v1));
        }
        else {
            SS_iter->second.push_back(s2);
        }

        SS_iter=StatStar.find(s2);
        if (SS_iter == StatStar.end()) {
            vector<int> v2;
            v2.push_back(s1);
            StatStar.insert(pair<int, vector<int> >(s2, v2));
        }
        else {
            SS_iter->second.push_back(s1);
        }

    }

    //扫描标记组2中的星，如果该星已被标记为1，则标记为状态2；并记录与之毗邻的另一颗星；
    //（组1和组2为相同组的情况如何处理？是否需要特殊情况考虑呢？）
    int MatchCnt = 0;
    for (int i = grouphead_[group2]; i < grouphead_[group2] + groupsize_[group2]; i++) {
        int b1, b2;
        b1 = stardata_[i].star1;
        b2 = stardata_[i].star2;

        if (Flag[b1] != 0) {
            Flag[b1] = 2;
            for (int j = 0; j < StatStar[b1].size(); j++) {
                matchgroup_[MatchCnt].star1 = b2;
                matchgroup_[MatchCnt].star2 = StatStar[b1][j];
                matchgroup_[MatchCnt].middle_star = b1;
                MatchCnt++;
            }
        }

        if (Flag[b2] != 0) {
            Flag[b2] = 2;
            for (int j = 0; j < StatStar[b2].size(); j++) {
                matchgroup_[MatchCnt].star1 = b1;
                matchgroup_[MatchCnt].star2 = StatStar[b2][j];
                matchgroup_[MatchCnt].middle_star = b2;
                MatchCnt++;
            }
        }
    }

    //在标记组3中寻找可以符合条件的星对组成匹配三角形。使用栈存储成功匹配的编号；
    stack<int> succes;
    for (int i = grouphead_[group3]; i < grouphead_[group3]+groupsize_[group3]; i++) {
        int c1,c2;
        c1 = stardata_[i].star1;
        c2 = stardata_[i].star2;
        for (int j = 0; j < MatchCnt; j++) {
            if ((matchgroup_[j].star1 == c1 && matchgroup_[j].star2 == c2) || (matchgroup_[j].star1 == c2 && matchgroup_[j].star2 == c1)) {
                //匹配成功，获得一组匹配三角形。标记j,或者直接输出j中的三个星的编号
                succes.push(j);
                Flag[matchgroup_[j].middle_star] = 3;
            }
        }
    }

    if (succes.empty()) {
        cout << "Matching Failled";
        //扩大组的范围，或换一组三角形匹配。
    }
    else {
        //从匹配成功的三角形中筛选出最终结果，需要结合星等数据选择，或者加入GPS等参考数据
        int result=-1;
        while (!succes.empty()) {
            int k = succes.top();
            //int sign = check(k,m1,m2,m3);
            //if (sign == 1) {
            //	result = k;
            //	break;
            //}
            result = k; break;
        }
        if (result == -1) {
            cout << "Error!";
        }
            //把三角形中的三颗星确定，使用角距区分。
        else {
            cout << "The answer'id in match group(all possible answer) is: "<<result << endl;
            cout <<"The answer is(star'id in SkyMap): "<< matchgroup_[result].middle_star << " " << matchgroup_[result].star1 << " " << matchgroup_[result].star2 << endl;
        }

    }
}

bool TriangleMatching::MCheck(int k, float m0, float m1, float m2, float deta) {


    return true;
}

int TriangleMatching::Check(int k, float m1, float m2, float m3) {


    return 0;
}