//
// Created by 70700 on 2019/3/3.
//

#include "Star.h"
#include "MyFunctions.h"

void StarPoint::change_coordinate(const StarPoint &new_center) {
    this->x -= new_center.x;
    this->y -= new_center.y;
    if(this->x>=360) this->x -= 360;
    else if(this->x < 0) this->x += 360;
    if(this->y<-90) this->y = -180-this->y;
    else if(this->y>90) this->y = 180-this->y;
}

bool StarPoint::XInRange(const StarPoint &center, double length) {
    //if(length >=360) return true;
    if(center.x + length/2 > 360){
        return ( (this->x > center.x-length/2) || (this->x<(center.x + length/2 - 360) ) );
    }else if(center.x - length/2 < 0){
        return ( (this->x < center.x+length/2) || (this->x>(center.x - length/2 + 360) ) );
    }else return ( (this->x > center.x-length/2)&&(this->x < center.x+length/2) );
}

bool StarPoint::YInRange(const StarPoint &center, double width) {
    return fabs(this->y-center.y) < width/2;
}

bool StarPoint::InRange(const StarPoint &center, double length, double width) {
    return this->XInRange(center,length) && this->YInRange(center,width);
}

bool StarPoint::PlaneInRange(const StarPoint &center, double length, double width) {
    //return InRange(&center, length, width);
    double p_length,p_width;
    p_length = length/cos(center.y*M_PI / 180);
    p_width = width;
    return InRange(center,p_length,p_width);
}
