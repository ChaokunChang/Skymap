#ifndef EXTRACTION_H
#define EXTRACTION_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <vector>

// macros
#define BLOCK 8
#define ALPHA 5
#define STARS_MAX 200
#define FOOTSTEP 40
#define LOCAL_BLOCK 3
#define LOCAL_NUM 30
#define MEAN_BLOCK 5
#define MEAN_MAX 100
#define GRADIENT_MAX 3

using namespace std;
using namespace cv;

// functions
vector<pair<int, int>> projection_row(Mat &segimg);
vector<pair<int, int>> projection_col(Mat &segimg);
Mat read_img(const string &filename);
double background_threshold(Mat &img);
double local_background_threshold(Mat &img);
Mat gradient_filter(Mat img);
Mat mean_filter(Mat img);
Mat threshold_filter(Mat &greyimg);
Mat grey_img(Mat &img);
Mat preprocess_img(Mat &greyimg, const string &filter);
void print_vector(vector<pair<pair<double, double>, double>> &v);
int block_sum_weight(int i, int j, pair<int, int> &px, pair<int, int> &py, const char &multiple);
double block_sum(Mat &img, pair<int, int> &px, pair<int, int> &py, const char &multiple, const double &T);
vector<pair<pair<double, double>, double>> get_centroids(Mat &img);
Mat plot_centroids(Mat plotimg, vector<pair<pair<double, double>, double>> &centroids, const uchar color[]);
Mat plot_centroids_white_background(Mat &img, vector<pair<pair<double, double>, double>> &centroids, const uchar color[]);
Mat star_filter(Mat &img, const int &max, const double &footstep);

#endif //EXTRACTION_H
