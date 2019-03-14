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
#include <math.h>
#include <stack>
#include <time.h>
#include "Star.h"
#include "MyFunctions.h"

using namespace std;
//星对存储结构
struct StarPair
{
    int group_number;//分组存储星对数据，分组依据为角距大小。首先星对按角距由小到大排序，然后没0.02°分一个组，便于索引

    int star1;//存储星对的两颗星的编号
    int star2;
    float angular_distance;//四位有效数字存储，且小于对焦距d（d 可取12°）

    StarPair(int s1, int s2, float dis):star1(s1),star2(s2),angular_distance(dis){};

};

struct MatchPair {
    int star1;
    int star2;
    int middle_star;
};


class TriangleMatching {
private:
    int GuideStarNumber, GroupNumber;
    float Threshold, GapWidth;
    MatchPair ChosedTriangle;
public:
    TriangleMatching(int starnum, float thres, float gap):GuideStarNumber(starnum),
                                                                        Threshold(thres),GapWidth(gap){ };
    vector<StarPair> stardata_;
    vector<MatchPair> matchgroup_;
    vector<int> grouphead_;
    vector<int> grouptail_;
    vector<int> groupsize_;
    void LoadData(vector<StarPoint> &stars);

    bool MCheck(int k, float m0, float m1, float m2,float deta);
    int Check(int k,float m1,float m2,float m3);
    float GetThreshold(){ return Threshold;};

    vector<StarPoint> ChooseTargetStars(vector<StarPoint> &obv_stars);
    vector<StarPoint> Random3Stars(vector<StarPoint> &obv_stars);
    void MatchAlgorithm(float ad12, float ad23, float ad13,float m1, float m2,float m3);


};


#endif //SKYMAP_TRIANGLEMATCHING_H
