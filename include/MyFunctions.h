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
#include <unordered_map>
#include <algorithm>
#include <QDebug>
#include <algorithm>
#include <random>
#include <time.h>
#include <QRandomGenerator>
#include <sys/timeb.h>
#include "extraction.h"
#define EPSINON 1e-6

extern struct timeb time_seed;
double cal_dis(const double& x1,const double& y1,const double& x2,const double& y2);
bool between(const double& target, const double& left, const double& right);
double getSpotAD(const double& x1, const double& y1, const double& x2, const double& y2, const double& f);
double random_double(const double& l, const double& r);
int random_int(const int& l,const int& r);
size_t random_size_t(const size_t& l,const size_t& r);
double getSphereAD(double, double, double, double);
double getSphereAngle(double, double, double, double, double, double);
double getSpotAngle(double, double, double, double, double, double,double);
std::pair<double,double> star2spot(double, double, double, double, double, double);
std::pair<double,double> LPT(double,double);
double inch2mm(double inch);
double mm2inch(double inch);
#endif //SKYMAP_MYFUNCTIONS_H
