#ifndef EXTRACTION_H
#define EXTRACTION_H

#include <iostream>
#include "lib/opencv-4.0.1/include/opencv2/opencv.hpp"
#include <vector>

// macros
#define BLOCK 5
#define ALPHA 5
#define STARS_MAX 200
#define FOOTSTEP 40

using namespace cv;
using namespace std;


// functions
vector<pair<int, int>> projection_row(Mat &segimg);
vector<pair<int, int>> projection_col(Mat &segimg);
Mat read_img(const string &filename);
double background_threshold(Mat &img);
Mat preprocess_img(Mat &srcimg);
void print_vector(vector<pair<double, double>> &v);
int block_sum_weight(int i, int j, pair<int, int> &px, pair<int, int> &py, const char &multiple);
double block_sum(Mat &img, pair<int, int> &px, pair<int, int> &py, const char &multiple);
vector<pair<double, double>> get_centroids(Mat &img);
Mat plot_centroids(Mat plotimg, vector<pair<double, double>> &centroids, const uchar color[]);
Mat plot_centroids_white_background(Mat &img, vector<pair<double, double>> &centroids, const uchar color[]);
Mat star_filter(Mat &img, const int &max, const double &footstep);

#endif //EXTRACTION_H
