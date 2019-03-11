//
// Created by 70700 on 2019/3/3.
//
#include "MyFunctions.h"

float cal_dis(float x1,float y1,float x2,float y2){
    //return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    return sqrt( pow(fmod(x1-x2,180),2) + pow(fmod(y1-y2,90),2) );
}

bool between(float target, float left, float right){
    if(target>=left && target<=right) return true;
    else return false;
}