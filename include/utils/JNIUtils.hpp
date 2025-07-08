//
// Created by Tony on 2025/7/8.
//

#ifndef MONICAIMAGEPROCESS_JNIUTILS_HPP
#define MONICAIMAGEPROCESS_JNIUTILS_HPP

#pragma once
#include <jni.h>
#include <functional>
#include <stdexcept>
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

    int channels = image.channels();
    if(channels == 3) {
        int size = image.total();
        jintArray resultImage = env->NewIntArray(size);
        jint *_data = new jint[size];
        for (int i = 0; i < size; i++) {
            char r = image.data[3 * i + 2];
            char g = image.data[3 * i + 1];
            char b = image.data[3 * i + 0];
            char a = (char)255;
            _data[i] = (((jint) a << 24) & 0xFF000000) + (((jint) r << 16) & 0x00FF0000) +
                       (((jint) g << 8) & 0x0000FF00) + ((jint) b & 0x000000FF);
        }
        env->SetIntArrayRegion(resultImage, 0, size, _data);
        delete[]_data;

        return resultImage;
    } else if (channels == 1) {
        int size = image.rows * image.cols;
        jintArray resultImage = env->NewIntArray(size);
        unsigned char *matData = image.data;
        jint *_data = new jint[size];
        for (int i = 0; i < size; i++) {
            _data[i] = static_cast<jint>(matData[i]);
        }
        env->SetIntArrayRegion(resultImage, 0, size, _data);
        delete[] _data;

        return resultImage;
    }
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

template <typename T>
T safeJniCall(JNIEnv* env, const std::function<T()>& nativeLogic, T fallbackValue) {
    try {
        return nativeLogic();
    } catch (const std::exception& e) {
        jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
        if (exceptionClazz != nullptr) {
            env->ThrowNew(exceptionClazz, e.what());
            env->DeleteLocalRef(exceptionClazz);
        }
    } catch (...) {
        jclass exceptionClazz = env->FindClass("java/lang/RuntimeException");
        if (exceptionClazz != nullptr) {
            env->ThrowNew(exceptionClazz, "Native error: Unknown exception");
            env->DeleteLocalRef(exceptionClazz);
        }
    }
    return fallbackValue;
}

#endif //MONICAIMAGEPROCESS_JNIUTILS_HPP
