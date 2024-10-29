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

typedef struct {
    bool showOriginalImage;
    bool showBoundingRect;
    bool showMinAreaRect;
    bool showCenter;
} ContourDisplaySettings;

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
void laplaceSharpening(Mat& src, Mat& dst);

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
void roberts(Mat image, Mat& dst);

/**
 * prewitt 算子
 */
void prewitt(Mat image, Mat& dst);

/**
 * sobel 算子
 */
void sobel(Mat image, Mat& dst);

/**
 * laplace 算子
 */
void laplace(Mat image, Mat& dst);

/**
 * LoG 算子
 */
void log(Mat image, Mat& dst);

/**
 * DoG 算子
 */
void dog(Mat image, Mat& dst, double sigma1, double sigma2, int size);

/**
 * canny 算子
 */
void canny(Mat image, Mat& dst, double threshold1, double threshold2, int apertureSize);

/**
 * inRange 颜色分割
 */
void inRange(Mat image, Mat& dst, int hmin, int smin, int vmin, int hmax, int smax, int vmax);

/**
 * 轮廓分析
 */
void contourAnalysis(Mat& src, Mat& binary, ContourDisplaySettings contourDisplaySettings);


#endif //MONICAIMAGEPROCESS_LIBRARY_H
