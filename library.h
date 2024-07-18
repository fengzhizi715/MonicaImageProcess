#ifndef MONICAIMAGEPROCESS_LIBRARY_H
#define MONICAIMAGEPROCESS_LIBRARY_H

#include <string>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

string getVersion();

string getOpenCVVersion();

Mat equalizeHistImage(Mat src);

void gammaCorrection(Mat& src, Mat& dst, float K);

void laplace(Mat& src, Mat& dst);

void unsharpMask(const Mat& src, Mat& dst, int radius, int threshold, int amount);

void ace(Mat& src, Mat& dst, int ratio, int radius);

#endif //MONICAIMAGEPROCESS_LIBRARY_H
