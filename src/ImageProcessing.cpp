//
// Created by 70700 on 2019/3/25.
//

#include "ImageProcessing.h"

vector<StarPoint> ImageProcessing::Process() {
    Mat srcimg, segimg;
    vector<pair<pair<double, double>,double>> centroids;

    // read srcimg
    cout << "Reading image..." << endl;
    srcimg = read_img(this->__raw_picture);

    // segmentation
    cout << "Preprocessing image..." << endl;
    segimg = preprocess_img(srcimg);

    // centroids
    cout << "Get centroids of stars..." << endl;
    centroids = get_centroids(segimg);

    vector<StarPoint> recList;
    for (size_t i=0;i!=centroids.size();i++) {
        StarPoint starRec(int(i),centroids[i].first.first,centroids[i].first.second,centroids[i].second);
        recList.push_back(starRec);
    }
    QCSVAdapter pic_csv(QString::fromStdString(this->__gen_picture));
    pic_csv.writeRecords(recList);
    return recList;
}
