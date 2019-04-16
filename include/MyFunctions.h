//
// Created by 70700 on 2019/3/3.
//

#ifndef SKYMAP_MYFUNCTIONS_H
#define SKYMAP_MYFUNCTIONS_H
#include <math.h>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <QDebug>

double cal_dis(double x1,double y1,double x2,double y2);
bool between(double target, double left, double right);
double getSpotAD(double x1, double y1, double x2, double y2, double f);
double random_double(double l, double r);
int random_int(int l,int r);
size_t random_size_t(size_t l, size_t r);
#endif //SKYMAP_MYFUNCTIONS_H
