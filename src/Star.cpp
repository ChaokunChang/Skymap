//
// Created by 70700 on 2019/3/3.
//

#include "Star.h"
#include "MyFunctions.h"
//Star::Star()
//{
//}

//Star::Star(int id, double x = 0, double y = 0, double mag = 0) :id(id), spx(x), spy(y), mag(mag)
//{
//}

//Star::~Star()
//{
//}

void StarPoint::change_coordinate(const StarPoint &new_center) {
    if(between(new_center.x, 0,360)&&between(new_center.y,-90,90)){
        double  dx = this->x-new_center.x;
        if(dx>180){
            this->x = dx - 360;
        }else if(dx < -180){
            this->x = dx + 360;
        } else{
            this->x = dx;
        }
        this->y = this->y-new_center.y;
    }
}

bool StarPoint::XInRange(const StarPoint &center, double length) {
    if(center.x + length/2 > 360){
        if( (this->x > center.x) || (this->x<(center.x + length/2 - 360) ) ) return true;
    }else if(center.x - length/2 < 0){
        if( (this->x < center.x) || (this->x>(center.x - length/2 + 360) ) ) return true;
    }else if( (this->x > center.x-length/2)&&(this->x < center.x+length/2) ) return true;
    else return false;
    return false;
}

bool StarPoint::YInRange(const StarPoint &center, double width) {
    return abs(this->y-center.y) < width/2;
}

bool StarPoint::InRange(const StarPoint &center, double length, double width) {
    return this->XInRange(center,length) && this->YInRange(center,width);
}
