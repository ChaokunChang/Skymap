#include "extraction.h"
#include <math.h>

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
    reduce(segimg, img, 1, REDUCE_SUM, CV_32S);

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
    reduce(segimg, img, 0, REDUCE_SUM, CV_32S);

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

double local_background_threshold(Mat &img)
{
    // calculate the background threshold of an image
    double mean = 0, stddev = 0, Vth;
    pair<int, int> px, py;
    vector<pair<int, int>> px_v, py_v;
    int height = img.rows - LOCAL_BLOCK, width = img.cols - LOCAL_BLOCK;
    int i, j, k;
    long pixels = LOCAL_BLOCK * LOCAL_BLOCK * LOCAL_NUM;

    // get mean and standard deviation of greyimg
//    Mat meanimg, stddevimg;
//    meanStdDev(img, meanimg, stddevimg);
//    mean = meanimg.at<double>(0, 0);
//    stddev = stddevimg.at<double>(0, 0);

    // mean
    for (i = 0; i < LOCAL_NUM; i++)
    {
        px.first = rand() % height;
        px.second = px.first + LOCAL_BLOCK;
        py.first = rand() % width;
        py.second = py.first + LOCAL_BLOCK;
        px_v.push_back(px);
        py_v.push_back(py);
        mean += block_sum(img, px, py, '1', 0);
    }
    mean /= pixels;

    // stddev
    for (i = 0; i < LOCAL_NUM; i++)
        for (j = px_v[i].first; j < px_v[i].second; j++)
            for (k = py_v[i].first; k < py_v[i].second; k++)
                stddev += pow(img.at<uchar>(j, k) - mean, 2);
    stddev = sqrt(stddev / (pixels - 1));

    // threshold formula
    Vth = mean + ALPHA * stddev;

    return Vth;
}

Mat gradient_filter(Mat img)
{
    Mat dx, dy; // 1st derivative in x,y
    Sobel(img, dx, CV_32F, 1,0);
    Sobel(img, dy, CV_32F, 0,1);

    Mat angle, mag;
    cartToPolar(dx, dy, mag, angle);
    int height = img.rows, width = img.cols;
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (mag.at<uchar>(i, j) < GRADIENT_MAX)
                img.at<uchar>(i, j) = 0;

    return img;
}

Mat mean_filter(Mat img)
{
    int length = img.rows, width = img.cols;
    pair<int, int> px, py;
    double threshold = MEAN_BLOCK * MEAN_BLOCK * MEAN_MAX;

    // mean
    for (int i = 0; i < length; i += MEAN_BLOCK)
        for (int j = 0; j < width; j += MEAN_BLOCK)
        {
            px.first = i;
            px.second = (i + MEAN_BLOCK < length) ? (i + MEAN_BLOCK) : length;
            py.first = j;
            py.second = (j + MEAN_BLOCK < width) ? (j + MEAN_BLOCK) : width;
            if (block_sum(img, px, py, '1', 0) > threshold)
                for (int m = px.first; m < px.second; m++)
                    for (int n = py.first; n < py.second; n++)
                        img.at<uchar>(m, n) = 0;
        }

    return img;
}

Mat threshold_filter(Mat &greyimg)
{
    // background threshold segmentation
    Mat segimg;
    double Vth;

    Vth = local_background_threshold(greyimg);
    cout << "T: " << Vth << endl;
    threshold(greyimg, segimg, Vth, 0, THRESH_TOZERO);
    return segimg;
}

Mat grey_img(Mat &img)
{
    // get greyimg
    if (img.channels() == 3)
    {
        Mat greyimg;
        cvtColor(img, greyimg, CV_BGR2GRAY);
        return greyimg;
    }
    else
        return img;
}

Mat preprocess_img(Mat &greyimg, const string &filter)
{
    // filter greyimg with different standard
    if (filter == "gradient")
        return gradient_filter(greyimg);
    else
    if (filter == "threshold")
        return threshold_filter(greyimg);
    else
        return mean_filter(greyimg);
}

void print_vector(vector<pair<pair<double, double>, double>> &v)
{
    // print a vector of pairs, and show its size
    long length = v.size();
    if (length)
    {
        long i;
        cout << "[" << "(" << v[0].first.first << ", " << v[0].first.second << ", " << v[0].second << ")";
        for (i = 1; i < length; i++)
        {
            cout << "," << endl;
            cout << "(" << v[i].first.first << ", " << v[i].first.second << ", " << v[i].second << ")";
        }
        cout << "]" << endl;
    }
    cout << "Size: " << length << endl;
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

double block_sum(Mat &img, pair<int, int> &px, pair<int, int> &py, const char &multiple, const double &T)
{
    // calculate the weighted sum of a block
    double sum = 0;
    int i, j;

    for (i = px.first; i < px.second; i++)
        for (j = py.first; j < py.second; j++)
            if (img.at<uchar>(i, j) >= T)
                sum += img.at<uchar>(i, j) * block_sum_weight(i, j, px, py, multiple);

    return sum;
}

vector<pair<pair<double, double>, double>> get_centroids(Mat &img)
{
    // locate the stars in an image, return a vector of coordinates

    vector<pair<pair<double, double>, double>> centroids;
    vector<pair<int, int>> x, y;
    pair<pair<double, double>, double> p;
    long length_x, length_y;
    int i, j;

    double T = background_threshold(img);

    // x projection: vertical range
    x = projection_row(img);

    // y projection: horizontal range
    y = projection_col(img);

    // x, y size
    length_x = x.size();
    length_y = y.size();
    cout << "Blocks: " << length_x << " x " << length_y << endl;

    for (i = 0 ; i < length_x; i++)
    {
        for (j = 0; j < length_y; j++)
        {
            // get denominator
            p.second = block_sum(img, x[i], y[j], '1', T);
            if (p.second == 0)
                continue;

            // get p.first
            p.first.first = block_sum(img, x[i], y[j], 'x', T);
            p.first.first = p.first.first / p.second + x[i].first;

            // get p.second
            p.first.second = block_sum(img, x[i], y[j], 'y', T);
            p.first.second = p.first.second / p.second + y[j].first;

//            double tmp = p.first.second;
//            p.first.second = p.first.first;
//            p.first.first = tmp;
            centroids.push_back(p);
        }
    }

    return centroids;
}

Mat plot_centroids(Mat plotimg, vector<pair<pair<double, double>, double>> &centroids, const uchar color[])
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
        x = int(round(centroids[i].first.first));
        y = int(round(centroids[i].first.second));

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

Mat plot_centroids_white_background(Mat &img, vector<pair<pair<double, double>, double>> &centroids, const uchar color[])
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
        stars = get_centroids(filimg).size();
        cout << ++count << " T: " << T << " " << stars << endl;
        T += footstep;
    } while (stars > max);

    return filimg;
}
