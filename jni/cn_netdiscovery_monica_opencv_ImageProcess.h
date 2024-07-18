//
// Created by Tony Shen on 2024/7/14.
//
#include <jni.h>

#ifndef MONICAIMAGEPROCESS_CN_NETDISCOVERY_MONICA_OPENCV_IMAGEPROCESS_H
#define MONICAIMAGEPROCESS_CN_NETDISCOVERY_MONICA_OPENCV_IMAGEPROCESS_H
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getVersion
        (JNIEnv* env, jobject);

JNIEXPORT jstring JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_getOpenCVVersion
        (JNIEnv* env, jobject);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_equalizeHist
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_gammaCorrection
        (JNIEnv* env, jobject,jbyteArray array, jfloat k);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_laplace
        (JNIEnv* env, jobject,jbyteArray array);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_unsharpMask
        (JNIEnv* env, jobject,jbyteArray array, jint radius, jint threshold, jint amount);

JNIEXPORT jintArray JNICALL Java_cn_netdiscovery_monica_opencv_ImageProcess_ace
        (JNIEnv* env, jobject,jbyteArray array, jint ratio, jint radius);

#ifdef __cplusplus
}
#endif
#endif
#pragma once