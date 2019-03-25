//
// Created by 70700 on 2019/3/25.
//

#include "ImageProcessing.h"

string ImageProcessing::Process() {
    Mat srcimg, segimg;
    vector<pair<double, double>> centroids;

    // read srcimg
    cout << "Reading image..." << endl;
    srcimg = read_img(f_name);

    // segmentation
    cout << "Preprocessing image..." << endl;
    segimg = preprocess_img(srcimg);

    // centroids
    cout << "Get centroids of stars..." << endl;
    centroids = get_centroids(segimg);

    fstream fout;

    fout.open("Data/data.csv", ios::out);
    int length = centroids.length();
    for (int i = 0; i < length; i++)
        fout << i << ',' << centroids[i].first << ',' << centroids[i].second << ',' << 0 << endl;
    fout.close();
    return this->__gen_picture;
}