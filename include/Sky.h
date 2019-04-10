//
// Created by 70700 on 2019/3/3.
//

#ifndef SKYMAP_SKY_H
#define SKYMAP_SKY_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

class Sky {
public:
    ifstream Stars;
    ifstream Image;
    void LoadStars(string f_name);
    void LoadImage(string f_name);

};


#endif //SKYMAP_SKY_H
