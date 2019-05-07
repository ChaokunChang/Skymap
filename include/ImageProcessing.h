//
// Created by 70700 on 2019/3/25.
//

#ifndef SKYMAP_IMAGEPROCESSING_H
#define SKYMAP_IMAGEPROCESSING_H

#include <string>
#include <fstream>
#include "extraction.h"
#include "QCSVAdapter.h"
#include <QImage>

using namespace std;
class ImageProcessing {
private:
    string __raw_picture;
    string __gen_picture;
public:
    ImageProcessing(string source_path, string target_path):__raw_picture(source_path),__gen_picture(target_path){};
    vector<StarPoint> Process();
};

QImage cvMat2QImage(const cv::Mat&);
cv::Mat QImage2cvMat(QImage);

#endif //SKYMAP_IMAGEPROCESSING_H
