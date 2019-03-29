#include <iostream>

//#include "ImageProcessing.h"
#include "SkyMapMatching.h"
#include <malloc.h>
using namespace std;

int main() {

//    ImageProcessing IP("../Data/source001.png","../Data/source001.csv");
//    string processed_picture = IP.Process();
//    if(processed_picture == "") return 0;

    SkyMapMatching SMM;
    string dataset = "../Data/skymaps.csv";
    string picture = "../Data/observation_00.csv";
    SMM.LoadSky(dataset);
    SMM.LoadImage(picture);
    StarPoint c(0,140.0,0.0,0.0);
    float len = 20.0,width = 20.0;
    SMM.GenerateSimImage(c,len,width);
    //SMM.GenerateSimImage(c,1,20);
    SMM.SelectTargetStar();
    SMM.Match();
    if( SMM.Check() ) cout<<"Good!"<<endl;
    else cout<<"Sad!"<<endl;

//    string filename = "images/stars1.jpg";
//    string output_file = "Data/centroids.csv";
//    cout << "start?" << endl;
//    SMM.LoadImage_img(filename, output_file);
//    cout << "finished?" << endl;

    return 0;
}