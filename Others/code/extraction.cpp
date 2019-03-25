//#include "extraction.h"

#include <stdfix.h>
#include <vector>
#include <iostream>
//#include "lib/opencv-4.0.1/include/opencv2/opencv.hpp"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

vector<pair<int, int>> projection_row(Mat &segimg)
{
    // project rows to get vertical range

    int height;
    Mat img;
    int i, count;
    vector<pair<int, int>> v;
    pair<int, int> p;

    height = segimg.rows;
    reduce(segimg, img, 1, CV_REDUCE_SUM, CV_32S);

    for (i = 0; i < height; )
    {
        if (img.at<uchar>(i, 0) > 0)
        {
            p.first = i;
            count = 0;
            while (count < BLOCK && i < height && img.at<uchar>(i, 0) > 0)
            {
                i++;
                count++;
            }
            p.second = i;
            v.push_back(p);
        }
        else
            i++;
    }

    return v;
}

vector<pair<int, int>> projection_col(Mat &segimg)
{
    // project cols to get horizontal range

    int width;
    Mat img;
    int i, count;
    vector<pair<int, int>> v;
    pair<int, int> p;

    width = segimg.cols;
    reduce(segimg, img, 0, CV_REDUCE_SUM, CV_32S);

    for (i = 0; i < width; )
    {
        if (img.at<uchar>(0, i) > 0)
        {
            p.first = i;
            count = 0;
            while (count < BLOCK && i < width && img.at<uchar>(0, i) > 0)
            {
                i++;
                count++;
            }
            p.second = i;
            v.push_back(p);
        }
        else
            i++;
    }

    return v;
}

Mat read_img(const string &filename)
{
    // read image, return source image

    Mat srcimg;
    srcimg = imread(filename);
    if (!srcimg.data)
    {
        cout << "Error loading image file: " << filename << endl;
        exit(1);
    }

    return srcimg;
}

double background_threshold(Mat &img)
{
    // calculate the background threshold of an image

    Mat meanimg, stddevimg;
    double mean, stddev, Vth;

    // get mean and standard deviation of greyimg
    meanStdDev(img, meanimg, stddevimg);
    mean = meanimg.at<double>(0, 0);
    stddev = stddevimg.at<double>(0, 0);

    // threshold formula
    Vth = mean + ALPHA * stddev;

    return Vth;
}

Mat preprocess_img(Mat &srcimg)
{
    // source image -> grey image -> segmented by a certain threshold

    Mat greyimg, segimg;
    double Vth;

    // get greyimg
    if (srcimg.channels() == 3)
        cvtColor(srcimg, greyimg, CV_BGR2GRAY);
    else
        greyimg = srcimg;

    // background threshold
    Vth = background_threshold(greyimg);

    // threshold segmentation
    threshold(greyimg, segimg, Vth, 0, THRESH_TOZERO);

    return segimg;
}

void print_vector(vector<pair<double, double>> &v)
{
    // print a vector of pairs, and show its size

    long length = v.size();
    long i;
    cout << "[" << "(" << v[0].first << ", " << v[0].second << ")";
    for (i = 1; i < length; i++)
    {
        cout << "," << endl;
        cout << "(" << v[i].first << ", " << v[i].second << ")";
    }
    cout << "]" << endl;
    cout << "Size: " << v.size() << endl;
}

int block_sum_weight(int i, int j, pair<int, int> &px, pair<int, int> &py, const char &multiple)
{
    // switch to the corresponding weight

    switch(multiple)
    {
        case 'x':
            return i - px.first;
        case 'y':
            return j - py.first;
        default:
            return 1;
    }
}

double block_sum(Mat &img, pair<int, int> &px, pair<int, int> &py, const char &multiple)
{
    // calculate the weighted sum of a block

    double sum = 0;
    int i, j;
    double T;

    // threshold
    T = background_threshold(img);

    for (i = px.first; i < px.second; i++)
        for (j = py.first; j < py.second; j++)
            if (img.at<uchar>(i, j) >= T)
                sum += img.at<uchar>(i, j) * block_sum_weight(i, j, px, py, multiple);

    return sum;
}

vector<pair<double, double>> get_centroids(Mat &img)
{
    // locate the stars in an image, return a vector of coordinates

    vector<pair<double, double>> centroids;
    vector<pair<int, int>> x, y;
    pair<double, double> p;
    long length_x, length_y;
    int i, j;
    double deno;

    // x projection: vertical range
    x = projection_row(img);

    // y projection: horizontal range
    y = projection_col(img);

    // x, y size
    length_x = x.size();
    length_y = y.size();

    for (i = 0 ; i < length_x; i++)
    {
        for (j = 0; j < length_y; j++)
        {
            // get denominator
            deno = block_sum(img, x[i], y[j], '1');
            if (deno == 0)
                continue;

            // get p.first
            p.first = block_sum(img, x[i], y[j], 'x');
            p.first = p.first / deno + x[i].first;

            // get p.second
            p.second = block_sum(img, x[i], y[j], 'y');
            p.second = p.second / deno + y[j].first;

            centroids.push_back(p);
        }
    }

    return centroids;
}

Mat plot_centroids(Mat plotimg, vector<pair<double, double>> &centroids, const uchar color[])
{
    // mark the centroids in a 3-channel image

    long length = centroids.size();
    long i, j;
    int x, y;
    int height, width;

    height = plotimg.rows;
    width = plotimg.cols;
    for (i = 0; i < length; i++)
    {
        x = int(round(centroids[i].first));
        y = int(round(centroids[i].second));

        // central point
        x = (x >= height)? (height - 1): x;
        y = (y >= width)? (width - 1): y;

        // plot the desired color
        for (j = 0; j < 3; j++)
        {
            plotimg.at<Vec3b>(x, y)[j] = color[j];

            // up, down, left, right points
            if (y + 1 < width)
                plotimg.at<Vec3b>(x, y + 1)[j] = color[j];
            if (y - 1 >= 0)
                plotimg.at<Vec3b>(x, y - 1)[j] = color[j];
            if (x + 1 < height)
                plotimg.at<Vec3b>(x + 1, y)[j] = color[j];
            if (x - 1 >= 0)
                plotimg.at<Vec3b>(x - 1, y)[j] = color[j];
        }
    }

    return plotimg;
}

Mat plot_centroids_white_background(Mat &img, vector<pair<double, double>> &centroids, const uchar color[])
{
    // plot the controids on a white background with the same size as img

    Mat plotimg(img.size(), CV_8UC3, Scalar(255, 255, 255));
    plotimg = plot_centroids(plotimg, centroids, color);

    return plotimg;
}

Mat star_filter(Mat &img, const int &max, const double &footstep)
{
    // filter stars under number max

    Mat filimg;
    int count = 0;
    long stars;
    double T = background_threshold(img);

    do
    {
        threshold(img, filimg, T, 0, THRESH_TOZERO);
        T += footstep;
        stars = get_centroids(filimg).size();
        cout << "Filtering stars " << ++count << ": " << stars << endl;
    } while (stars > max);

    return filimg;
}