//
// Created by Tony on 2025/7/10.
//

#ifndef MONICAIMAGEPROCESS_JNI_UTILS_H
#define MONICAIMAGEPROCESS_JNI_UTILS_H

#pragma once

#include <jni.h>
#include <opencv2/opencv.hpp>
#include <functional>
#include <stdexcept>

cv::Mat byteArrayToMat(JNIEnv* env, jbyteArray array);

cv::Mat byteArrayTo8UC1Mat(JNIEnv* env, jbyteArray array);

jintArray matToIntArray(JNIEnv* env, const cv::Mat& mat);

jintArray binaryMatToIntArray(JNIEnv* env, cv::Mat bin);

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

#endif //MONICAIMAGEPROCESS_JNI_UTILS_H
