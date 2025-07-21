//
// Created by Tony on 2025/7/10.
//
#include "jni_utils.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

Mat byteArrayTo8UC1Mat(JNIEnv* env, jbyteArray array) {
    jsize len = env->GetArrayLength(array);
    signed char* pData = new  signed char[len];
    env->GetByteArrayRegion(array, 0, len, pData);

    // 解码内存数据，变成cv::Mat数据
    cv::Mat image;
    vector<uchar> datas;
    for (int i = 0; i < len; ++i) {
        datas.push_back(pData[i]);
    }
    image = cv::imdecode(datas, IMREAD_GRAYSCALE);

    return image;
}

Mat byteArrayToMat(JNIEnv* env, jbyteArray array) {
    //复制java数组到C++
    jsize len = env->GetArrayLength(array);
    signed char* pData = new  signed char[len];
    env->GetByteArrayRegion(array, 0, len, pData);

    // 解码内存数据，变成cv::Mat数据
    cv::Mat image;
    vector<uchar> datas;
    for (int i = 0; i < len; ++i) {
        datas.push_back(pData[i]);
    }
    image = cv::imdecode(datas, IMREAD_COLOR);

    return image;
}

jintArray matToIntArray(JNIEnv *env, const cv::Mat &image) {
    int width = image.cols;
    int height = image.rows;
    int channels = image.channels();

    int size = width * height;
    jintArray resultImage = env->NewIntArray(size);
    jint *argb = new jint[size];

    if (channels == 3) {
        for (int y = 0; y < height; ++y) {
            const uchar* row = image.ptr<uchar>(y);
            for (int x = 0; x < width; ++x) {
                int idx = y * width + x;
                uchar b = row[x * 3 + 0];
                uchar g = row[x * 3 + 1];
                uchar r = row[x * 3 + 2];
                argb[idx] = (0xFF << 24) | (r << 16) | (g << 8) | b;
            }
        }
    } else if (channels == 1) {
        for (int y = 0; y < height; ++y) {
            const uchar* row = image.ptr<uchar>(y);
            for (int x = 0; x < width; ++x) {
                int idx = y * width + x;
                uchar gray = row[x];
                argb[idx] = (0xFF << 24) | (gray << 16) | (gray << 8) | gray;
            }
        }
    } else {
        delete[] argb;
        return nullptr;  // Unsupported format
    }

    env->SetIntArrayRegion(resultImage, 0, size, argb);
    delete[] argb;
    return resultImage;
}

jintArray binaryMatToIntArray(JNIEnv *env, const cv::Mat binary) {

    int width = binary.cols;
    int height = binary.rows;
    int size = width * height;

    jintArray resultImage = env->NewIntArray(size);
    jint* intArray = new jint[size];
    for (int i = 0; i < size; i++) {
        intArray[i] = (binary.data[i] == 255) ? 0xFFFFFF : 0x000000;  // 0xFFFFFF 表示白色，0x000000 表示黑色
    }
    env->SetIntArrayRegion(resultImage, 0, size, intArray);
    delete[] intArray;

    return resultImage;
}