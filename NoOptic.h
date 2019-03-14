//
// Created by 70700 on 2019/3/3.
//

#ifndef SKYMAP_NOOPTIC_H
#define SKYMAP_NOOPTIC_H

#include<iostream>
#include<vector>
#include<math.h>
#include "MyFunctions.h"

#define R0 2
//#define Rn 15
#define SightSize 15
#define PartitionNumber 2800 //35 * 80
#define StarNumber 6000
#define Pi 3.1415926

using namespace std;

struct StarPattern{
    int number;
    vector<int> star_array;
};

struct EigenVecStruct{
    int star_index;
    vector<bool> eigen_vec;
};

class NoOptic{
    /************************************************************************************
    无标定参数的星途识别
    A:建立导航星表
        1.每一刻导航星都要做一次主星，并查找其周围一定区域范围内的临星（距离的计算无需使用角距，
        直接使用距离计算就可以），用于建立特征
        2.在临星中选定定位星，作为基建立极坐标系。其长度为单位长度，角度为0；选取是不一定要选最
        近的那个，可以选择在某个界限之外最近的那个，这样可以提高精度。
        3.极坐标系下的分区操作，构造M*N的特征向量pat(S),方格内有星则为1，否则为0
        4.Attention, Please! 这个步骤很奇怪，用了一种奇怪的方式存储星模式，和一般的思维相反。
        为M*N个分区各分配一个序列，存储主星序号，如果某一个主星在该位置有临星存在，则把该主星的
        序号存入这个序列中。序列按升序排列，便于查找。

    B:匹配识别
        1.选主星，定临星，并分区M*N。
        2.为每一个导航星分配一个计数器
        3.如果某一块区域i中有临星，则在星模式中找到第i行，该行对应导航星序列的导航星的计数器+1。
        4.遍历导航星的计数器，数值最大的与观测星匹配。
        //以上为初步匹配，获得的匹配结果一般不唯一
        5.快速分组确定最终结果，这个过程也蛮奇怪的，他为每个子块分配了一个计数器，如果有一块待选
        星属于该子块，则该子块的计数器+1，寻找计数器最大值所在的子块，一般来说，在该子块及其周
        围8各子块内能够找到和观测星数量相同的待选星。

    C:仿真参考
        1.径向比例半径的选取M=35（径向），N=80（环向）

    *************************************************************************************/
public:
    /*变量区*/
    vector< pair<float,float> > SkyStars; //导航星库中的星
    vector< pair<float,float> > ImageStars; //观测星图中的星
    vector<EigenVecStruct> StarEigens;

    //vector<StarPattern> GridStorage;//星模式序列存储
    StarPattern PartitionCounter[PartitionNumber];

    int FinalResult;

    int ExeNoOptic();//总执行流程

    /* get the eigenvector */
    vector<bool> GetEigenVector(int StarNum);
    int GetMidStar();
    bool Match(int main_star);

};

#endif //SKYMAP_NOOPTIC_H
