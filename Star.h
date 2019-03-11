//
// Created by 70700 on 2019/3/3.
//

#ifndef SKYMAP_STAR_H
#define SKYMAP_STAR_H

struct StarPoint{
    int index;
    float x; //0 -> 360
    float y; //-90 -> 90
    float magnitude;
    StarPoint(int i,float x, float y,float mag):index(i),x(x),y(y),magnitude(mag){};
};


class Star {
public:
    Star();
    Star(int, double, double, double);
    ~Star();
    void operator=(const Star &source){
        id = source.id;
        spx = source.spx;
        spy = source.spy;
        mag = source.mag;
    }
    int getID() { return id; }
    double getX() { return spx; }
    double getY() { return spy; }
    double getMag() { return mag; }
    //bool operator< (const Star& a) const { return a.id < id; }
private:
    int id;
    double spx;
    double spy;
    double mag;
};


#endif //SKYMAP_STAR_H
