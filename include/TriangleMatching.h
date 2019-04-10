//
// Created by 70700 on 2019/3/3.
//

#ifndef SKYMAP_TRIANGLEMATCHING_H
#define SKYMAP_TRIANGLEMATCHING_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <stack>
#include <ctime>
#include <random>
#include "Star.h"
#include "MyFunctions.h"

using namespace std;
//星对存储结构
struct StarPair
{
    // int group_number;//分组存储星对数据，分组依据为角距大小。首先星对按角距由小到大排序，然后每0.02°分一个组，便于索引
    int star1;//存储星对的两颗星的编号
    int star2;
    double angular_distance;//四位有效数字存储，且小于对焦距d（d 可取12°）

    StarPair(int s1, int s2, double dis):star1(s1),star2(s2),angular_distance(dis){};

};

struct MatchPair {
    int star1;
    int star2;
    int middle_star;
};


class TriangleMatching {
private:
    int __GuideStarNumber, __GroupNumber;
    double __Threshold, __GapWidth;
    //vector<StarPoint> __TargetTriangle;
    MatchPair __TargetTriangle;
    stack<int> __Candidate;
public:
    TriangleMatching(int starnum, double thres, double gap):__GuideStarNumber(starnum),
                                                                        __Threshold(thres),__GapWidth(gap){ };
    vector<StarPair> stardata_;
    vector<MatchPair> matchgroup_;
    vector<int> grouphead_;
    vector<int> grouptail_;
    vector<int> groupsize_;
    void LoadData(vector<StarPoint> &stars);

    double GetThreshold(){ return this->__Threshold;};

    void ChooseAdjacentStars(vector<StarPoint> &obv_stars, vector<StarPoint> &triangle);
    vector<StarPoint> RandomAdjacentStars(vector<StarPoint> &obv_stars, int except);
    int MatchAlgorithm(double center_edge1, double center_edge2, double edge1_edge2, double m1, double m2,double m3);

};


#endif //SKYMAP_TRIANGLEMATCHING_H
