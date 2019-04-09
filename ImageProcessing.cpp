//
// Created by 70700 on 2019/3/25.
//

#include "ImageProcessing.h"

vector<pair<double, double>> ImageProcessing::Process() {
    Mat srcimg, segimg;
    vector<pair<double, double>> centroids;

    // read srcimg
    cout << "Reading image..." << endl;
    srcimg = read_img(this->__raw_picture);

    // segmentation
    cout << "Preprocessing image..." << endl;
    segimg = preprocess_img(srcimg);

    // centroids
    cout << "Get centroids of stars..." << endl;
    centroids = get_centroids(segimg);

    fstream fileout;
    fileout.open(this->__gen_picture, ios::out);
    int length = centroids.size();
    for (int i = 0; i < length; i++)
        fileout << i << ',' << centroids[i].first << ',' << centroids[i].second << ',' << 0 << endl;
    fileout.close();
    return centroids;
}
