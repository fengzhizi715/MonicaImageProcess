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

/**
 * 图像错切
 * @param 沿 x 方向
 * @param 沿 y 方向
 */
Mat shearing(Mat src, float x, float y);

/**
 * 直方图均衡化
 */
Mat equalizeHistImage(Mat src);

/**
 * 限制对比度自适应直方图均衡
 */
void clahe(Mat& src, Mat& dst, double clipLimit, int size);

/**
 * gamma 校正
 */
void gammaCorrection(Mat& src, Mat& dst, float K);

/**
 * laplace 锐化，主要是 8 邻域卷积核
 */
void laplace(Mat& src, Mat& dst);

/**
 * USM 锐化
 */
void unsharpMask(const Mat& src, Mat& dst, int radius, int threshold, int amount);

/**
 * 自动色彩均衡
 */
void ace(Mat& src, Mat& dst, int ratio, int radius);

/**
 * roberts 算子
 */
void roberts(Mat gray, Mat& dst);

/**
 * prewitt 算子
 */
void prewitt(Mat gray, Mat& dst);

/**
 * sobel 算子
 */
void sobel(Mat gray, Mat& dst);


#endif //MONICAIMAGEPROCESS_LIBRARY_H
