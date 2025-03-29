//
// Created by Tony on 2025/3/29.
//

#ifndef MONICAIMAGEPROCESS_JNIUTILS_H
#define MONICAIMAGEPROCESS_JNIUTILS_H

#include "opencv2/opencv.hpp"
#include <jni.h>

using namespace std;
using namespace cv;

cv::Mat byteArrayTo8UC1Mat(JNIEnv* env, jbyteArray array);

cv::Mat byteArrayToMat(JNIEnv* env, jbyteArray array);

jintArray matToIntArray(JNIEnv *env, const cv::Mat &image);

jintArray binaryMatToIntArray(JNIEnv *env, const cv::Mat binary);

#endif //MONICAIMAGEPROCESS_JNIUTILS_H
