#include "SkyMap.h"

void SkyMapInterface::run(){
	ImageProcessing IP("../Data/source001.png","../Data/source001.csv");
    vector<pair<double, double>> centroids = IP.Process();
    if(centroids.empty()) return;

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
    if( SMM.Check()>=0 ) cout<<"Good!"<<endl;
    else cout<<"Sad!"<<endl;
}
